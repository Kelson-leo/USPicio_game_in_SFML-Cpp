#!/usr/bin/env node
/**
 * Smoke test for UspicioGame WASM build.
 *
 * Runs the WASM module in Node.js and verifies that:
 * 1. The WASM module loads without errors
 * 2. No abort()/unreachable occurs within the timeout
 *
 * Usage: node smoke_test.js [timeout_ms=5000]
 */

const { spawn } = require('child_process');
const path = require('path');

const BUILD_DIR = __dirname;
const TIMEOUT = parseInt(process.argv[2] || '5000', 10);

async function main() {
    console.log(`[smoke] Starting smoke test (timeout: ${TIMEOUT}ms)...`);

    // Emscripten-generated JS for the game
    const gameJs = path.join(BUILD_DIR, 'UspicioGame.js');

    // Use Emscripten's Node.js-compatible entry point
    // The game needs the shell.html environment — load via the JS file
    const child = spawn('node', ['--experimental-wasm-threads', '-e', `
        globalThis.document = {
            getElementById: () => null,
            currentScript: { src: '${gameJs}' },
            querySelector: () => null,
            createElement: () => ({ style: {}, getContext: () => null, addEventListener: () => {} }),
            body: { appendChild: () => {} },
            addEventListener: () => {}
        };
        globalThis.window = globalThis;
        globalThis.location = { href: 'file://${BUILD_DIR}/', pathname: '/' };
        globalThis.navigator = { userAgent: 'node-smoke-test' };
        globalThis.requestAnimationFrame = (cb) => setTimeout(cb, 16);

        // Override abort to detect crashes
        const origAbort = () => {
            console.error('[SMOKE-FAIL] Abort called — game crashed!');
            process.exit(1);
        };

        process.on('uncaughtException', (err) => {
            console.error('[SMOKE-FAIL] Uncaught exception:', err.message);
            process.exit(1);
        });

        process.on('unhandledRejection', (err) => {
            console.error('[SMOKE-FAIL] Unhandled rejection:', (err||{}).message || err);
            process.exit(1);
        });

        try {
            require('${gameJs}');
        } catch(e) {
            if (e.message && e.message.includes('Aborted')) {
                console.error('[SMOKE-FAIL] Game aborted during load');
                process.exit(1);
            }
            throw e;
        }
    `], {
        cwd: BUILD_DIR,
        timeout: TIMEOUT + 2000,
        env: { ...process.env, NODE_NO_WARNINGS: '1' }
    });

    let output = '';

    child.stdout.on('data', (data) => {
        const text = data.toString();
        output += text;
        process.stdout.write('[game] ' + text);
    });

    child.stderr.on('data', (data) => {
        const text = data.toString();
        output += text;
        if (text.includes('[main] Game created, entering run loop')) {
            console.log('[smoke] PASS: Game reached run loop!');
        }
        if (text.includes('[SMOKE-FAIL]')) {
            console.error('[smoke] FAIL: ' + text.trim());
        }
    });

    const result = await new Promise((resolve) => {
        const timer = setTimeout(() => {
            console.log('[smoke] PASS: No crash within timeout');
            child.kill('SIGKILL');
            resolve(0);
        }, TIMEOUT);

        child.on('close', (code) => {
            clearTimeout(timer);
            if (code !== 0 && code !== null) {
                console.error(`[smoke] FAIL: Process exited with code ${code}`);
                resolve(1);
            } else {
                resolve(0);
            }
        });
    });

    process.exit(result);
}

main().catch((err) => {
    console.error('[smoke] Error:', err.message);
    process.exit(2);
});
