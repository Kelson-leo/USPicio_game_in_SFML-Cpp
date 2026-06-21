# CLAUDE.md — Uspício Game

## 1. Dados Gerais

- **Projeto:** Uspício Game
- **Gênero:** Plataforma/Luta 2D Lateral (Side-scrolling beat 'em up)
- **Tema:** Estudante da USP vs. Capivaras e Professores
- **Stack:** C++17, SFML 2.6.2, CMake 3.20+, Google Test 1.14 (TDD)
- **Diretório Raiz:** `~/Documentos/Programming/Cpp_Language/Games/Uspicio_game`
- **Executável:** `UspicioGame`
- **Assets:** `assets/` relativo ao executável

## 2. Arquitetura (Não Negociável)

### Hexagonal (Ports & Adapters)

```
                    ┌──────────────────────────────────┐
                    │           CORE (Domain)           │
                    │  NO #include <SFML/...>           │
                    │  Pure C++17 interfaces only       │
                    │                                   │
                    │  IRenderer      IInputHandler     │
                    │  IAudioPlayer   ITextureLoader    │
                    │  IGameObject    IGameState        │
                    └──────────┬───────────────────────┘
                               │ depends on (DIP)
                    ┌──────────▼───────────────────────┐
                    │     INFRASTRUCTURE (Adapters)     │
                    │  #include <SFML/...> allowed      │
                    │                                   │
                    │  SfmlRenderer   SfmlInput         │
                    │  SfmlAudio      SfmlTextureLoader │
                    └──────────────────────────────────┘
```

**Regra de Ouro:** Nenhum arquivo em `src/core/` pode conter `#include <SFML/...>`. As interfaces devem usar tipos próprios (ex: `core::Color`, `core::Event`, `core::KeyCode`) ou tipos padrão C++. Os adapters em `src/infrastructure/` traduzem entre os tipos Core e SFML.

### SOLID

- **SRP:** Cada classe tem uma única razão para mudar.
- **OCP:** Entidades estendidas via herança/Strategy, não modificadas.
- **LSP:** Subtipos substituíveis sem quebrar o código cliente.
- **ISP:** Interfaces pequenas e específicas (ex: `IRenderer` não obriga áudio).
- **DIP:** Camadas superiores dependem de abstrações (ports), nunca de concretizações (adapters). Injeção de dependência por construtor.

### Design Patterns Obrigatórios

| Pattern | Onde | Por quê |
|---|---|---|
| **State** | Player, Capivara, Professor (Idle, Walking, Attacking, Hurt, Dead) | Comportamento distinto por estado |
| **Factory Method** | Spawn de Capivaras, Projéteis, Chefão (Professor) | Criação desacoplada das fases |
| **Strategy** | Ataques: `SocoStrategy`, `CanetaStrategy` (Player); `LivroStrategy` (Professor) | Diferentes comportamentos de ataque |
| **Singleton (controlado)** | `AssetManager` | Cache de texturas, com injeção via construtor nos consumidores |

## 3. Regras de Negócio (Game Design)

### Vidas
- 5 corações globais. Persistem entre Fase 1, 2 e 3.
- Cada dano sofrido remove 1 coração.

### Fases (Sprint 10 — 7 fases configuráveis via JSON)

| Fase | Nome | Inimigos | Chefão | Chão |
|---|---|---|---|---|
| 1 | Pátio | 2 Capivaras | — | 900 |
| 2 | Bandeijão | 3 Capivaras | **Rato** | 900 |
| 3 | InterUSP | 4 Capivaras | — | 900 |
| 4 | Busão | 3 Capivaras | **Mandrake** | 900 |
| 5 | SanFran | 4 Capivaras | **Peru** + **Bau** | 900 |
| 6 | Biblioteca | 5 Capivaras | — | 900 |
| 7 | Reitoria | 2 Capivaras | **Professor** | 990 |

### Chefes (Sprint 10)

Hierarquia: classe base `Boss` → subclasses `Professor`, `Rato`, `Mandrake`, `Peru`.

| Chefe | Fase | Ranged | Melee | Projétil | Dano Ranged | Dano Melee |
|---|---|---|---|---|---|---|
| **Rato** | 2 (Bandeijão) | Panela (250px/s) | — | `ProjectileType::Pan` | 10 | — |
| **Mandrake** | 4 (Busão) | Pedra (300px/s) | Soco/porrada | `ProjectileType::Stone` | 15 | 8 |
| **Peru** | 5 (SanFran) | Copo (200px/s) | Bicada (1.2s cd) | `ProjectileType::Cup` | 8 | 6 |
| **Professor** | 7 (Reitoria) | Prova (250px/s) | — | `ProjectileType::Exam` | 12 | — |

- **Boss** base: `update()` dual-mode — se `m_canMelee && dx <= 150`: melee attack; senão se `dx <= 600`: ranged attack.
- `getProjectileType()` virtual → cada subclasse retorna seu tipo.
- `performMeleeAttack(Player&)` virtual → Mandrake e Peru sobrescrevem.
- `getHeight()` virtual → Rato sobrescreve (97.5px); demais usam BOSS_HEIGHT (80px).
- **Rato:** Idle (1 frame) + Attack (5 frames, toca uma vez ao disparar). 345×390 native, escala 0.35 → ~121×136.
- **Mandrake:** Idle v1 (1 frame) ou v2 (2 frames em loop, 50% chance, 0.3s/frame). Re-sorteia idle após ranged. Ranged: 5 frames. Melee: 2 frames. 154-286×318 native, escala 0.5 → ~77-143×159.
- Fábrica em `Game::loadLevel()` por `chefao_tipo`.

### Progressão (Sprint 8)

- Ao derrotar todos os inimigos da fase atual, o jogo avança automaticamente para a próxima.
- Após a última fase → tela de **"PARABENS!"** (vitória).
- Vidas e munição persistem entre fases (transição preserva estado do Player).
- **Restart:** reinicia da Fase 1 com estado limpo (5 vidas, 10 munição).
- **Restart Phase:** reinicia a fase atual preservando vidas e munição.

### Game Over (Sprint 10)

- Fases sem chefão (1, 3, 6) com 0 vidas → **"REPROVADO"**.
- Fases com chefão (2, 4, 5, 7) com 0 vidas → **"JUBILADO"**.
- Lógica: `phase.hasBoss` → "JUBILADO", senão "REPROVADO".
- Vitória após derrotar o chefão da Fase 7 (Reitoria) → **"Formado!"**.

### Física
- **Chão:** Y = 880 (tela 1080p). Altura do tile de chão: 200px.
- **Gravidade:** Padrão (valor a definir).
- **Pulo:** Buffer de entrada (input buffering) para melhor jogabilidade.

### Tela
- Resolução base: 1920x1080.
- Viewport fixa ou câmera com scroll lateral (a decidir).

## 4. Tipos Próprios do Core (Sprint 1.1)

Para garantir que `src/core/` não inclua SFML, foram criados tipos próprios no namespace `core`:

| Tipo | Header | Descrição |
|---|---|---|
| `Color` | `IRenderer.h` | `{r,g,b,a}` uint8_t, com factories `Black()`, `White()`, `Red()`, `Green()`, `Blue()`, `Transparent()` |
| `Vector2f` | `IRenderer.h` | `{x,y}` float |
| `Vector2u` | `IRenderer.h` | `{x,y}` unsigned int |
| `FloatRect` | `IRenderer.h` | `{left,top,width,height}` float |
| `Drawable` | `Drawable.h` | Interface abstrata: `void draw(IRenderer&) const` |
| `KeyCode` | `IInputHandler.h` | Enum: A-Z, Space, Enter, Escape, Left, Right, Up, Down |
| `EventType` | `IInputHandler.h` | Enum: Closed, KeyPressed, KeyReleased |
| `Event` | `IInputHandler.h` | `{EventType type; KeyCode key;}` |

A tradução Core ↔ SFML fica em `src/infrastructure/SfmlConversions.h` (header-only, funções inline `toSfml()` / `toCore()`).

## 5. Constantes Físicas

Definidas em `src/core/PhysicsConstants.h` (resolução de referência: 1080p):

| Constante | Valor | Unidade |
|---|---|---|
| `GROUND_Y` | 900.0f | pixels (topo do tile de chão — medido no Inkscape) |
| `GROUND_HEIGHT` | 126.0f | pixels (medido no Inkscape) |
| `GRAVITY` | 800.0f | pixels/s² |
| `JUMP_SPEED` | -400.0f | pixels/s (negativo = para cima) |

### UI Asset Scaling (Sprint 2)

Assets de UI têm tamanhos nativos variados e precisam de escala para manter proporção:

| Asset | Dimensão nativa | Escala | Tamanho final |
|---|---|---|---|
| `heart.png` | 500×500 | 0.04 (20/500) | ~20×20 px |
| `caneta.png` | 500×500 | A definir | A definir (Sprint de projéteis) |
| `livro.png` | 500×500 | A definir | A definir (Sprint de projéteis) |

Espaçamento entre corações: 42px (10px de gap). Constantes definidas em `Game.cpp`:
```cpp
constexpr float HEART_SCALE = 20.0f / 500.0f;   // 0.04 → 20px
constexpr float HEART_STEP  = 30.0f;             // 10px gap
```

## 6. Nomenclatura de Assets

```
assets/
├── fonts/
│   └── PressStart2P.ttf        (Google Fonts, baixada via script)
├── backgrounds/
│   ├── fase1_patio.png
│   ├── fase2_interusp.png
│   ├── fase3_biblioteca.png
│   └── fase4_reitoria.png
├── tiles/
│   ├── tile_grama.png
│   └── tile_madeira.png
├── sprites/
│   ├── player/
│   │   └── player_sheet.png
│   ├── capivara/
│   │   └── capivara_sheet.png   (Frames: 0=Idle, 1=Walk1, 2=Walk2, 3=Hit, 4=Dead)
│   └── professor/
│       └── professor_sheet.png  (Frames: 0=Idle, 1=Atk1, 2=Atk2, 3=Hit, 4=Dead)
├── projectiles/
│   ├── caneta.png
│   └── livro.png
├── sounds/
│   └── music/
│       └── background_music.mp3  (optional, game runs without it)
└── ui/
    └── heart.png
```

## 7. Padrões de Código e Build

### Língua
- **Código:** Inglês (classes, variáveis, métodos, comentários).
- **UI/Mensagens:** Português ("USPICIO", "REPROVADO", "JUBILADO", menus).

### TDD
- Testes unitários (Google Test) escritos **ANTES** da implementação funcional.
- Cobertura obrigatória para toda regra de negócio.
- Mock/Stub quando necessário para isolar dependências externas (SFML).

### Keyboard Controls

| Key | Action | Context |
|---|---|---|
| `Arrow Keys` | Move left/right | Gameplay |
| `Space` / `Up` | Jump | Gameplay (on ground) |
| `Z` | Punch (range ≤ 80px) | Gameplay |
| `X` | Throw Pen (costs 1 ammo) | Gameplay |
| `C` | Defend (blocks 70% projectile damage) | Gameplay (hold) |
| `Down Arrow` | Crouch | Gameplay (hold) |
| `Enter` | Select menu option / Start game | Menu |
| `Up` / `Down` | Navigate menu options | Menu |
| `Escape` | Pause (during gameplay) / Back (Info) / Exit (Main menu) | Gameplay / Info / Menu |

### Menu (Sprint 5 → 9)

Four menu options navigable with Up/Down/Enter:
- **Start** — Loads Phase 1 and starts gameplay
- **Options** — Music/SFX volume sliders (Left/Right to adjust, +/-5%)
- **Info** — Shows developer info, copyright, and keyboard controls overlay
- **Restart** — Resets all state and reloads Phase 1

### Options Screen (Sprint 9)

- Full overlay (700×430, dark).
- **Music Volume:** slider bar with Left/Right (±5%).
- **Sound Effects Volume:** same (placeholder for future SFX).
- **Background Music:** `< Track N/4 >` — Left/Right switches tracks immediately. 4 tracks from `assets/sounds/music/background_music1..4.mp3`.
- Music volume applied immediately via `AudioManager::setMusicVolume()`.
- Navigation: Up/Down selects item, Left/Right adjusts value, Enter on "Back" or ESC returns to Main.

### Audio System (Sprint 9)

- **AudioConfig** (`core/AudioConfig.h`): struct with `musicVolume`, `effectVolume` (0–100) and `selectedTrack` (0–3). SFML-free.
- **AudioManager** (`infrastructure/AudioManager.h`): adapter wrapping `sf::Music`. Supports 4 tracks: `assets/sounds/music/background_music1..4.mp3`. Methods: `loadTrack(index)`, `nextTrack()`, `previousTrack()`, `getCurrentTrackLabel()`.
- If a track file is missing, logs warning and disables music for that track.
- **Track selector** in Options: "< Track N/4 >" with Left/Right to switch. Music changes immediately.
- Music plays from menu and continues throughout gameplay.
- Build: requires `sfml-audio` linked in CMakeLists.

### Info Screen (Sprint 5)

- Full overlay (800×600, dark) hides menu items behind
- Developer name, copyright (c) 2026 Kelson
- Controls list with double-line spacing
- "Back" option selectable with Enter (or Esc)

### Pause Menu (Sprint 5 → 9)

Pressionar `ESC` durante o jogo abre o menu de pausa sobreposto ao jogo congelado:
- **Resume** — Volta ao jogo (ESC também funciona)
- **Options** — Abre o menu de opções (volume, track selector) sobre o jogo pausado. Back/ESC retorna ao pause
- **Restart** — Reinicia a fase atual, resetando inimigos e posição do player (mantém vidas e munição)
- **Quit to Menu** — Sai para o menu principal, resetando todo o progresso

Navegação: Up/Down para selecionar, Enter para confirmar.
**Options é acessível tanto do menu principal quanto do menu de pausa** — as configurações são globais e persistem.

### Crouch (Sprint 8)

Estado de agachamento acionado ao pressionar `Down Arrow` durante o jogo:
- **Animação:** 8 novos estados direcionais no `frames.json`: `crouch_idle`, `crouch_punch` (2 frames), `crouch_throw` (1 frame), `crouch_defend` (1 frame) × right/left.
- **Hitbox:** Reduzida verticalmente (`CROUCH_HEIGHT = 79 × 1.5 = 118.5px` vs `PLAYER_HEIGHT = 142.5px`).
- **Física:** Os pés permanecem em `GROUND_Y`. Ao agachar, `m_position.y` é ajustado para compensar a altura menor. Gravidade e movimento horizontal inalterados.
- **Pulo bloqueado:** `Space`/`Up` não têm efeito enquanto agachado.
- **Ataques:** `Z` (punch), `X` (throw), `C` (defend) funcionam com animações `crouch_*`.
- **Soco (Z):** Em pé → atinge apenas Professor (chefão). Agachado → atinge capivara na frente **com proximidade** (dx<70px, dy<100px, direção correta); se nenhuma em range, atinge Professor. **Cooldown:** 0.3s (`PUNCH_COOLDOWN`).
- **Caneta (X):** Escala 1.5× (99×19.5px). Offset Y dinâmico: 20 (em pé) / 30 (agachado). Em pé → passa por cima da capivara. Agachado → atinge capivara e Professor.
- **Dano unificado:** Soco e caneta usam `Capivara::takeDamage()` (única porta de dano, evita zumbis com HP=0).
- **Vector safety:** `m_capivaras.reserve()` antes de criar `HealthBar`s (evita dangling references por realocação).
- **Movimento:** Funciona (usa `crouch_idle` pois não há `crouch_walk`).
- `buildAnimName()` prefixa `"crouch_"` quando `m_isCrouching == true`.
- `getCurrentHeight()` retorna a altura apropriada para física e colisão.

### Capivara (Enemy — Sprint 6)

**Comportamento:** Anda automaticamente em direção ao player com velocidade baixa (40 px/s). Inverte direção ao atingir as bordas da tela (0 ou 1860). Começa no lado direito.

**Atributos:**
- HP: 30 | Speed: 40 px/s | Scale: 1.5× | Attack range: 30px
- Frame height: 55px nativa → 82.5px scaled
- Animations: idle, walk (2 frames), hurt, dead — cada com suffix `_right`/`_left`

**Dano por contato:** Proximidade < 30px + `|dy| < 80px` → `EnemyTouch` (8 dano base, NÃO redutível por defesa — defesa só protege contra projéteis).

**Contagem por fase:** Fase 1 = 2, Fase 2 = 3, Fase 3 = 2 + Professor.

### Projectile System (Sprint 7)

**Tipos:** `ProjectileType::Pen` (Player), `Exam` (Professor), `Pan` (Rato), `Stone` (Mandrake), `Cup` (Peru).

| Propriedade | Pen | Exam | Pan | Stone | Cup |
|---|---|---|---|---|---|
| Velocidade | 500 | 250 | 250 | 300 | 200 px/s |
| Dano | 20 | 12 | 10 | 15 | 8 |
| Lifetime | 3.0s | 3.0s | 3.0s | 3.0s | 3.0s |
| Escala | 1.5× | 1.0× | 0.6× | 0.8× | 0.6× |
| Dono | Player | Professor | Rato | Mandrake | Peru |

**Frames** (`assets/config/frames.json` → `projectiles`):
- `pen_left` (66×13 → ~99×20), `pen_right` (66×13), `exam_left` (62×67 → ~93×100)
- `panela_left` (137×79 → ~82×47), `panela_right` (137×79)
- `copo_left` (123×69 → ~74×41), `copo_right` (123×69)
- `pedra_left` (38×29 → ~30×23), `pedra_right` (38×29)
- Coords obtidas via `convert <img> -trim` (auto-trim das bordas transparentes dos PNGs 200×200)
- Tamanhos alvo: panela/copo ~80×45px, pedra ~30×23px (proporcionais à caneta ~60×20px)
- `_right` usa `_left` com `setScale(-scale, scale)` (espelhamento + escala unificada)

**Ciclo de vida:**
- Criado via `std::make_unique<Projectile>()`, adicionado ao `std::vector<std::unique_ptr<Projectile>>` no Game
- `update(dt)`: move por `velocity * dt`, decrementa lifetime, desativa se lifetime ≤ 0
- Colisão: Pen vs inimigos (Capivara/Professor), Exam vs Player
- Limpeza: `erase + remove_if` remove projéteis inativos a cada frame

**Disparo pelo Player** (`Player::throwProjectile`):
- Verifica `ammo.canUse()` e `m_shootCooldown <= 0`
- Cooldown: 0.3s entre disparos (`SHOOT_COOLDOWN`), evita gasto instantâneo
- Consome 1 munição, cria Pen na direção do player
- Offset Y: **-30** (em pé) / **25** (agachado). Offset X: **40** (direita) / **-10** (esquerda)
- **Comportamento:** Em pé → caneta passa por cima da capivara (base 797 < topo 817.5). Agachado → caneta acerta (base 831 > topo 817.5). Contra Professor funciona em ambos os estados (Professor é mais alto).

**Disparo pelos Chefes** (`Boss::shootProjectile`):
- Cada chefe armazena sua própria textura de projétil via `setProjectileTexture()`:
  - Rato → `assets.getTexture("panela")`, Professor → `assets.getTexture("exam")`
  - Mandrake → `assets.getTexture("pedra")`, Peru → `assets.getTexture("copo")`
- Dispara se vivo, cooldown ≤ 0, e distância ao player entre 80–600px (min 80px evita spawn dentro do player)
- MELEE_RANGE = 150px (Mandrake/Peru só socam se o player estiver muito próximo)
- Spawn offset: ±80px horizontal. Y offset: -10 (default), 0 (Mandrake — pedra mais baixa). Configurável via `getProjectileOffsetY()` virtual.
- **Período de graça:** 0.1s — projétil não pode colidir nos primeiros 100ms (evita destruição antes do primeiro draw)
- Reseta cooldown para 2.0s após cada disparo

### Chest / Bau (Sprint 11)

Baú interativo exclusivo da Fase 5 (Sanfran). Restaura vidas e munição ao ser tocado.

- **Localização:** Fase 5, `x = 500`, `y = groundY - CHEST_HEIGHT`
- **Uso único:** Abre no primeiro toque e permanece aberto (frame "open")
- **Condição de ativação:** `THRESHOLD = 0.8f` — só abre se vidas < 80% do max OU munição < 80% do max
  - 4/5 vidas (80%) e 8/10 munição (80%) → NÃO abre (igual ao threshold, não abaixo)
  - 3/5 vidas (60%) → abre mesmo com munição cheia
  - 7/10 munição (70%) → abre mesmo com vidas cheias
- **Bônus:** `HEAL_PERCENT = 0.8f` (80%) das vidas e munição perdidas, arredondado com `std::ceil`
  - Ex: perdeu 2 vidas → `ceil(2 * 0.8) = 2` restauradas
  - Ex: perdeu 1 vida → `ceil(1 * 0.8) = 1` restaurada
- **Frames:** `"closed"` (86,124,267×223), `"open"` (422,102,265×267) — escala 0.3 → ~80×67 (fechado) / ~80×80 (aberto)
- **Textura:** `assets/sprites/chest/chest_sheet.png`
- **Colisão:** Bounding-box (`CHEST_WIDTH=80, CHEST_HEIGHT=80`) vs Player bounds
- **Integração:** `m_chest` (unique_ptr) em Game, criado em `loadLevel()` se `m_currentPhase == 4`

### HUD / UI Display (Sprint 7)

**LivesDisplay** (corações, topo-esquerda):
- Desenha 5 slots (`maxLives`). Vivos: sólidos. Perdidos: opacidade 50 + X vermelho `(220,20,20)`.
- Contorno escuro ao redor de cada ícone (4 offsets de 1px).
- Fundo semi-transparente `(255,255,255,35)` atrás da fileira.

**AmmoDisplay** (canetas, abaixo dos corações em Y=40):
- Desenha 10 slots (`maxAmmo`). Disponíveis: sólidas. Usadas: opacidade 50 + X vermelho.
- Mesmo estilo visual: contorno escuro + fundo leve.
- Textura dedicada: `assets/projectiles/caneta_disp.png` (≈63×62 nativa, scale 0.30).
- Cooldown de disparo do Player (0.3s) evita gastar toda munição em 1 frame.

### Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cd build && ctest
```

### AssetManager (Sprint 2)

Singleton (`infrastructure::AssetManager::instance()`) que centraliza o carregamento de texturas:
- Armazena `std::unordered_map<std::string, std::unique_ptr<SfmlTextureLoader>>`
- `loadTexture(id, path)` → carrega do disco ou cria placeholder magenta 32x32 (fallback) e loga aviso
- `getTexture(id)` → retorna `const sf::Texture&`; IDs nunca carregados recebem fallback global
- `SfmlTextureLoader` implementa `core::ITextureLoader` internamente
- IDs oficiais: `heart`, `player`, `capivara`, `professor`, `caneta`, `caneta_disp`, `exam`, `livro`, e dinâmicos `bg_fase1_patio`, `bg_fase2_interusp`, `bg_fase3_biblioteca`, `bg_fase4_reitoria` (gerados por `Level` a partir do path)

### Level System (Sprint 2 → 8)

`gameplay::Level(const std::string& bgPath)` carrega background diretamente do path configurado.
- `draw(IRenderer&)` → estica background para preencher a janela
- `getGroundY()` → retorna `core::GROUND_Y`
- O ID do AssetManager é derivado automaticamente do path (ex: `assets/backgrounds/fase1_patio.png` → `bg_fase1_patio`)

### PhaseConfig (Sprint 8)

`gameplay::PhaseConfig` carrega `assets/config/fases.json` e fornece acesso tipado à configuração de fases:
- `loadFromFile(path)` → carrega JSON com nlohmann/json
- `size()` → número de fases
- `getPhase(index)` → `const PhaseData&` com `{id, background, enemyCount, enemyType, hasBoss, bossType}`
- `getBackground(index)`, `getEnemyCount(index)`, `hasBoss(index)`, `getBossType(index)` → convenience methods
- Índices fora de range retornam sentinel vazio (id=0, strings vazias, counts=0)

**Ground Y per Phase (Sprint 8):**
- Campo `ground_y` no `fases.json` define a altura do chão por fase (default: 900).
- Fases 1-3: `ground_y = 900`. Fase 4 (Reitoria): `ground_y = 990`.
- `Level` recebe `groundY` no construtor e `getGroundY()` retorna o valor configurado.
- Entidades (`Player`, `Capivara`, `Professor`) têm `setGroundY(float)` chamado em `loadLevel()`, reposicionando-as ao nível correto.
- `core::GROUND_Y` serve apenas como fallback/default; entidades usam `m_groundY` interno.

**Estrutura do JSON** (`assets/config/fases.json`):
```json
{
  "fases": [
    { "id": 1, "background": "...", "inimigos_comuns": 2, "tipo_inimigo": "capivara", "chefao": false },
    { "id": 4, "background": "...", "inimigos_comuns": 2, "tipo_inimigo": "capivara", "chefao": true, "chefao_tipo": "professor" }
  ]
}
```

**PhaseData** (value object, header-only em `PhaseConfig.h`):
```cpp
struct PhaseData {
    int id = 0;
    std::string background;
    int enemyCount = 0;
    std::string enemyType;
    bool hasBoss = false;
    std::string bossType;
};
```

### SfmlSprite / SfmlText (Sprint 2)

Adapters que implementam `core::Drawable` encapsulando `sf::Sprite` e `sf::Text`.
- `SfmlSprite::draw(IRenderer&)` → `dynamic_cast<SfmlRenderer&>(r).drawSfml(m_sprite)` — o cast fica contido em infrastructure/
- `SfmlText` — análogo para texto
- **Resolve TD-01**: Game agora usa `core::Drawable` sem `dynamic_cast` direto

### Sprite Outline / Drop Shadow (Sprint 8)

Player e Capivara desenham contorno escuro para destacar do background:
- **Técnica:** Drop shadow 8-directional — sprite desenhado 8 vezes com offset de 1px em `(0,0,0,180)` antes do sprite original.
- **Implementação:** Nos métodos `draw()` de `Player` e `Capivara`.
- `SfmlSprite::m_sprite` é `mutable` para permitir mudanças visuais temporárias em método `draw() const`.
- `getSfmlSprite()` const retorna `sf::Sprite&` não-const.

### FrameConfig (Sprint 2.5)

Sistema de configuração de frames via JSON (`assets/config/frames.json`):
- `infrastructure::FrameConfig` carrega JSON com estrutura `character → animation → [{x,y,w,h}]`
- Suporta frames com larguras diferentes (ex: soco 80px, idle 64px)
- `getFrame(char, anim, idx)` retorna `sf::IntRect`; índice fora de range → último frame
- `frameCount(char, anim)` retorna número de frames; chaves desconhecidas → 0
- Dependência: `nlohmann/json` (FetchContent, header-only, v3.11.3)

**Animações Player (dados reais Inkscape — 12 estados direcionais):**
- idle_right/left (1 frame, 40×95), walk_right/left (4 frames, 61-77×94-96), punch_right/left (3 frames, 61-74×95)
- jump_right/left (1 frame, 77×96), defend_right/left (1 frame, 60×97), throw_right/left (1 frame, 73×97)
**Capivara:** idle, walk (2), hurt, dead (placeholder — pendente dados reais)
**Professor:** idle (80×80), attack (2×100×80), hurt, dead (placeholder — pendente dados reais)

### Component System (Sprint 3)

Componentes de gameplay no Core (SFML-free, header-only):

| Componente | Header | Campos | Métodos |
|---|---|---|---|
| `HealthComponent` | `core/HealthComponent.h` | `int maxHP=100, currentHP=100` | `takeDamage(amount, isDefending=false)`, `heal(amount)`, `isDead()` |
| `LivesComponent` | `core/LivesComponent.h` | `int maxLives=5, currentLives=5` | `loseLife()`, `isGameOver()` |
| `AmmoComponent` | `core/AmmoComponent.h` | `int maxAmmo=10, currentAmmo=10` | `canUse()`, `use()`, `reload(amount)` |

### Damage System (Sprint 3)

Sistema de dano configurável via `core::DamageConfig` (mapa em código, extensível para JSON):

**Tipos** (`core/DamageTypes.h`):
- `AttackType`: Punch, Throw, EnemyTouch, BossProjectile
- `EntityType`: Player, Capivara, Professor

**Tabela de dano base** (valores padrão em `core/DamageConfig.cpp`):

| AttackType | vs. Capivara | vs. Professor | vs. Player |
|---|---|---|---|
| `Punch` | 10 | 5 | — |
| `Throw` | 20 | 15 | — |
| `EnemyTouch` | — | — | 8 |
| `BossProjectile` | — | — | 12 |

**Defesa:** Quando `isDefending=true` e o ataque é do tipo `BossProjectile`, dano é reduzido em 70% — player toma 30% (`max(1, ceil(amount * 0.3f))`). Não protege contra ataques corpo a corpo (`EnemyTouch`, soco do Mandrake, bicada do Peru).
**Morte do Player:** HP=0 → perde 1 vida → revive com HP cheio e posição inicial. Se vidas=0 → Game Over.

**Como ajustar valores:** Use `DamageConfig::setDamage(atk, ent, amount)` ou edite diretamente `DamageConfig.cpp`.

### UI Components (Sprint 3)

Todas em `src/infrastructure/`, namespace `infrastructure`, implementam `core::Drawable`:

| Classe | Construtor | Descrição |
|---|---|---|
| `HealthBar` | `(HealthComponent&, Vector2f size={100,10})` | Retângulo vermelho (fundo) + verde (HP). Não desenha se morto. |
| `LivesDisplay` | `(LivesComponent&, const sf::Texture&, scale=0.04, step=30)` | Ícones de coração em linha. |
| `AmmoDisplay` | `(AmmoComponent&, const sf::Texture&, scale=0.04, step=30)` | Ícones de caneta em linha. |

**IMPORTANTE — Vector Safety (Sprint 8):** `HealthBar` armazena `HealthComponent&` (referência). As capivaras vivem em `std::vector<Capivara>` que realoca ao crescer. Para evitar dangling references, `loadLevel()` faz `m_capivaras.reserve(enemyCount)` antes de criar as HealthBars.

### Entities (Sprint 3 → 4)

Entidades em `src/gameplay/` implementam `core::Drawable` para renderização:

| Classe | Componentes | HP Padrão | Métodos |
|---|---|---|---|
| `Player` | health, lives, ammo | 100 | `punch()`, `throwCaneta()`, `defend(bool)`, `takeHit()`, `revive()`, `moveLeft/Right()`, `applyGravity()`, `setAnimation()`, `updateAnimation()`, `setDirection()`, `setCrouching()`, `isCrouching()`, `getCurrentHeight()` |
| `Capivara` | health | 30 | `touchPlayer(Player&)` |
| `Professor` | health | 80 | `shootBook(Player&)`, `setGroundY()`. Feet at groundY via `PROFESSOR_HEIGHT=80`. Position: X=1700 (right side). |

### Animation & Rendering System (Sprint 4)

**Direction** (`core/Direction.h`): `enum class Direction { Left, Right }` com helper `opposite(Direction)`.

**Sistema de animação (Player):**
- `Player` herda `core::Drawable` — renderizado via `renderer.draw(*m_player)`
- Construtor recebe `sf::Texture&` + `FrameConfig&`
- `buildAnimName(action)` → concatena `action + "_" + (right ? "right" : "left")`
- `setAnimation(action)` → busca frames no FrameConfig, reseta frameIndex/timer. Fallback: mantém anterior se não encontrar.
- `updateAnimation(dt)` → avança `frameTimer`; a cada `FRAME_DURATION` (0.15s) avança frame.
  - **Walk:** loop circular (0→1→2→3→0)
  - **Demais ações:** para no último frame (não loop)
- `setDirection(dir)` → reaplica animação atual com sufixo direcional oposto
- Estados suportados: idle, walk (4 frames), punch (3), jump (1), defend (1), throw (1) — cada um com `_right`/`_left`

**Renderização (Game):**
- Level → sprites dos inimigos → sprite do player → health bars → HUD
- `FrameConfig` carregado no construtor do Game e injetado nas entidades
- Player começa em `idle_right`, posição (100, GROUND_Y)

**Input de gameplay (Playing state):**
- Left/Right → move + walk animation
- Space/Up → jump (só se on ground)
- Z → punch + dano no inimigo mais próximo
- X → throw + dano + usa ammo
- C → defend (isDefending=true)
- Soltar tudo → idle

**Física:**
- `applyGravity(dt)` → `velocityY += GRAVITY * dt`, clampa em `GROUND_Y`
- `moveLeft/Right(dt)` → `WALK_SPEED * dt` (200 px/s)
- Colisão com chão: `m_position.y >= GROUND_Y` → `velocityY = 0`
- Dano por proximidade: `|dx| < 60 && |dy| < 60` → `touchPlayer()`

**Frames do Player (dados reais do Inkscape):**
12 estados em `assets/config/frames.json`, com coordenadas medidas:
- idle/walk/punch/jump/defend/throw × right/left
- Walk: 4 frames (w: 61, 73, 74, 77)
- Punch: 3 frames (w: 61, 69, 74)
- Jump/Defend/Throw: 1 frame cada
- **Player scale:** 1.5× (`Player::PLAYER_SCALE`) aplicada no construtor via `m_sprite.setScale()`
- Jump/Defend/Throw: 1 frame cada

## 8. Fluxo de Comunicação

1. Toda sessão começa relendo `CLAUDE.md`.
2. Antes de implementar qualquer Sprint, entregar **Review Técnico** ao PO.
3. Só codificar após autorização explícita ("Aprovado").
4. Atualizar `CLAUDE.md` sempre que novas regras ou decisões arquiteturais forem tomadas.

---

## 9. Débitos Técnicos Conhecidos

| ID | Descrição | Impacto | Resolução planejada |
|---|---|---|---|
| TD-01 | ~~`Game::render()` faz `dynamic_cast<SfmlRenderer&>`~~ | — | **Resolvido Sprint 2:** `SfmlSprite`/`SfmlText` implementam `core::Drawable`; cast fica em infrastructure/ |
| TD-02 | `SfmlInput` armazena `sf::RenderWindow&` (não usa porta `IRenderer`) | Acoplamento direto ao SFML no adapter de input | Sprint 3 |

---

## Histórico de Sprints

| Sprint | Data | Descrição |
|---|---|---|
| 0 | 2026-06-19 | Setup inicial: CMake, estrutura de diretórios, ports, adapters, Game loop, menu placeholder |
| 1.1 | 2026-06-19 | Refatoração arquitetural: remoção de SFML do Core, tipos próprios, ITextureLoader, PhysicsConstants, SfmlConversions, fonte PressStart2P.ttf, 11 testes passando |
| 2 | 2026-06-19 | AssetManager, SfmlTextureLoader, SfmlSprite, SfmlText, Level, menu com background + corações, TD-01 resolvido, 16/16 testes |
| 2.5 | 2026-06-20 | FrameConfig com JSON (nlohmann/json), suporte a frames de largura variável, animações para player/capivara/professor, 29/29 testes |
| 3 | 2026-06-20 | HealthComponent, LivesComponent, AmmoComponent, DamageConfig com AttackType/EntityType, UI bars (HealthBar, LivesDisplay, AmmoDisplay), entidades esqueleto (Player, Capivara, Professor), integração no Game, 68/68 testes |
| 4 | 2026-06-20 | Animation & Rendering System: frames reais do Player (12 estados direcionais), Direction enum, sistema de animação (setAnimation, updateAnimation, buildAnimName), sprites nas 3 entidades, input de gameplay, física (gravidade/pulo/movimento), dano por proximidade, 78/78 testes |
| 5 | 2026-06-20 | Menu system (Start/Restart/Info with Up/Down/Enter navigation), Info screen (developer, copyright, controls overlay), Player scale 1.5x, keyboard controls documented, Restart fully resets game state |
| 6 | 2026-06-20 | Capivara enemy: real sprite frames (8 directional), animation system, AI movement toward player, edge clamping, contact damage, hurt/dead states, Fase 1=2, Fase 2=3, Fase 3=2+Professor |
| 7 | 2026-06-20 | Projectile system: Pen (player, 500px/s, 20dmg) and Exam (professor, 250px/s, 12dmg), collision with enemies/player, professor AI shoots at range 600px with 2s cooldown, unique_ptr lifecycle with erase_if cleanup |
| 8 | 2026-06-20 | Phase system restructure + Sprite outline/drop-shadow + Crouch state (8 anims, reduced hitbox, contextual punch/pen targeting) + Bugfixes: punch cooldown (0.3s), unified damage via Capivara::takeDamage(), vector reserve, pen scale 1.5x, dynamic groundY per phase (JSON-configurable), Professor ground alignment, victory message "Formado!", 92/92 tests |
| 9 | 2026-06-20 | Audio system: background music (sf::Music, looped, 4-track selector), AudioConfig/AudioManager, Options menu (volume sliders + track selector), Options accessible from Pause menu, 92/92 tests |
| 10 | 2026-06-21 | Expand to 7 phases + 4 bosses: Boss base class with Professor/Rato/Mandrake/Peru subclasses, factory in loadLevel(), placeholder spritesheets, 95/95 tests |
| 11 | 2026-06-21 | Chest power-up in Phase 5 (Sanfran): restores 80% lost lives/ammo (ceil), single-use, frame switching, 107/107 tests |
