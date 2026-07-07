#!/usr/bin/env node
/**
 * Playwright integration test for UspicioGame WASM.
 *
 * Requirements: npm install playwright
 *
 * Launches a real browser, loads the game, verifies:
 * 1. Canvas renders (no crash on load)
 * 2. No JS errors in console
 * 3. Takes screenshot for visual verification
 *
 * Usage:
 *   cd build-wasm && python3 -m http.server 8001 &
 *   node ../tests/smoke/playwright_test.js [--url=http://localhost:8001/UspicioGame.html]
 */

const { chromium } = require('playwright');
const path = require('path');

const GAME_URL = process.argv.find(a => a.startsWith('--url='))?.split('=')[1]
    || 'http://localhost:8001/UspicioGame.html';
const TIMEOUT = 15000;

async function main() {
    console.log(`[playwright] Launching browser to test: ${GAME_URL}`);

    const errors = [];
    const logs = [];

    const browser = await chromium.launch({
        args: [
            '--enable-features=SharedArrayBuffer',
            '--enable-blink-features=SharedArrayBuffer',
            '--disable-web-security'
        ]
    });

    const context = await browser.newContext({
        viewport: { width: 1920, height: 1080 }
    });

    const page = await context.newPage();

    // Collect console messages
    page.on('console', (msg) => {
        const text = msg.text();
        logs.push(`[${msg.type()}] ${text}`);
        if (msg.type() === 'error' && !text.includes('favicon.ico')) {
            errors.push(text);
        }
    });

    page.on('pageerror', (err) => {
        errors.push(`PAGE ERROR: ${err.message}`);
    });

    try {
        console.log('[playwright] Loading game page...');
        await page.goto(GAME_URL, { waitUntil: 'domcontentloaded', timeout: 30000 });

        // Wait for canvas to appear
        console.log('[playwright] Waiting for canvas...');
        await page.waitForSelector('canvas.emscripten', { timeout: 10000 });
        console.log('[playwright] Canvas found');

        // Wait for game to initialize (look for console output)
        console.log('[playwright] Waiting for game initialization...');
        try {
            await page.waitForFunction(() => {
                // Check if the game console output indicates success
                const textarea = document.getElementById('output');
                return textarea && textarea.value.includes('enter');
            }, { timeout: TIMEOUT });
        } catch {
            console.log('[playwright] Timeout waiting for game log — checking for errors');
        }

        // Check canvas dimensions
        const canvasBox = await page.evaluate(() => {
            const canvas = document.querySelector('canvas.emscripten');
            if (!canvas) return null;
            return { width: canvas.width, height: canvas.height };
        });
        console.log('[playwright] Canvas size:', JSON.stringify(canvasBox));

        // Take screenshot
        const screenshotPath = path.join(__dirname, 'screenshot.png');
        await page.screenshot({ path: screenshotPath, fullPage: true });
        console.log(`[playwright] Screenshot saved: ${screenshotPath}`);

        // Check for console errors (ignore favicon, AudioContext warnings)
        const realErrors = errors.filter(e =>
            !e.includes('favicon') &&
            !e.includes('AudioContext')
        );

        if (realErrors.length > 0) {
            console.error('[playwright] FAIL: Console errors found:');
            realErrors.forEach(e => console.error('  -', e));
            await browser.close();
            process.exit(1);
        }

        // Check if unreachable/abort occurred
        const fatalErrors = logs.filter(l =>
            l.includes('unreachable') ||
            l.includes('Aborted') ||
            l.includes('RuntimeError')
        );
        if (fatalErrors.length > 0) {
            console.error('[playwright] FAIL: Fatal errors in console:');
            fatalErrors.forEach(e => console.error('  -', e));
            await browser.close();
            process.exit(1);
        }

        console.log('[playwright] PASS: Game loaded and running without errors!');
    } catch (err) {
        console.error('[playwright] Error:', err.message);
        await page.screenshot({ path: path.join(__dirname, 'error.png'), fullPage: true });
        await browser.close();
        process.exit(1);
    }

    await browser.close();
    process.exit(0);
}

main();
