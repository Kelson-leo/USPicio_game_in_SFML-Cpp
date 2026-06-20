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

### Fases

| Fase | Nome | Dificuldade | Inimigos | Tile |
|---|---|---|---|---|
| 1 | Pátio | Fácil | 3 Capivaras | Grama |
| 2 | Biblioteca | Médio | 4 Capivaras | Madeira |
| 3 | Reitoria | Difícil | 2 Capivaras + Chefão (Professor) | Grama |

### Game Over
- **Fase 1 ou 2** com 0 vidas → Tela vermelha + som dramático + texto **"REPROVADO"** → Menu Principal.
- **Fase 3** com 0 vidas → Tela vermelha + som dramático + texto **"JUBILADO"** → Menu Principal.

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
│   ├── fase2_biblioteca.png
│   └── fase3_reitoria.png
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
| `Z` | Punch | Gameplay |
| `X` | Throw Pen (costs 1 ammo) | Gameplay |
| `C` | Defend (halves incoming damage) | Gameplay (hold) |
| `Enter` | Select menu option / Start game | Menu |
| `Up` / `Down` | Navigate menu options | Menu |
| `Escape` | Pause (during gameplay) / Back (Info) / Exit (Main menu) | Gameplay / Info / Menu |

### Menu (Sprint 5)

Three menu options navigable with Up/Down/Enter:
- **Start** — Loads Phase 1 and starts gameplay
- **Restart** — Resets all state (HP, lives, ammo, position, enemies) and reloads Phase 1
- **Info** — Shows developer info, copyright, and keyboard controls overlay

### Info Screen (Sprint 5)

- Full overlay (800×600, dark) hides menu items behind
- Developer name, copyright (c) 2026 Kelson
- Controls list with double-line spacing
- "Back" option selectable with Enter (or Esc)

### Pause Menu (Sprint 5)

Pressionar `ESC` durante o jogo abre o menu de pausa sobreposto ao jogo congelado:
- **Resume** — Volta ao jogo (ESC também funciona)
- **Restart** — Reinicia a fase atual, resetando inimigos e posição do player (mantém vidas e munição)
- **Quit to Menu** — Sai para o menu principal, resetando todo o progresso

Navegação: Up/Down para selecionar, Enter para confirmar.

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
- IDs oficiais: `background_fase1`, `background_fase2`, `background_fase3`, `tile_grama`, `tile_madeira`, `player`, `capivara`, `professor`, `caneta`, `livro`, `heart`

### Level System (Sprint 2)

`gameplay::Level(int phaseNumber)` carrega background e tile pelo AssetManager.
- `draw(IRenderer&)` → estica background para preencher a janela, desenha tile do chão em Y=GROUND_Y com altura GROUND_HEIGHT
- `getGroundY()` → retorna `core::GROUND_Y`

### SfmlSprite / SfmlText (Sprint 2)

Adapters que implementam `core::Drawable` encapsulando `sf::Sprite` e `sf::Text`.
- `SfmlSprite::draw(IRenderer&)` → `dynamic_cast<SfmlRenderer&>(r).drawSfml(m_sprite)` — o cast fica contido em infrastructure/
- `SfmlText` — análogo para texto
- **Resolve TD-01**: Game agora usa `core::Drawable` sem `dynamic_cast` direto

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

**Defesa:** Quando `isDefending=true`, dano é reduzido à metade (`max(1, amount/2)`).
**Morte do Player:** HP=0 → perde 1 vida → revive com HP cheio e posição inicial. Se vidas=0 → Game Over.

**Como ajustar valores:** Use `DamageConfig::setDamage(atk, ent, amount)` ou edite diretamente `DamageConfig.cpp`.

### UI Components (Sprint 3)

Todas em `src/infrastructure/`, namespace `infrastructure`, implementam `core::Drawable`:

| Classe | Construtor | Descrição |
|---|---|---|
| `HealthBar` | `(HealthComponent&, Vector2f size={100,10})` | Retângulo vermelho (fundo) + verde (HP). Não desenha se morto. |
| `LivesDisplay` | `(LivesComponent&, const sf::Texture&, scale=0.04, step=30)` | Ícones de coração em linha. |
| `AmmoDisplay` | `(AmmoComponent&, const sf::Texture&, scale=0.04, step=30)` | Ícones de caneta em linha. |

### Entities (Sprint 3 → 4)

Entidades em `src/gameplay/` implementam `core::Drawable` para renderização:

| Classe | Componentes | HP Padrão | Métodos |
|---|---|---|---|
| `Player` | health, lives, ammo | 100 | `punch()`, `throwCaneta()`, `defend(bool)`, `takeHit()`, `revive()`, `moveLeft/Right()`, `applyGravity()`, `setAnimation()`, `updateAnimation()`, `setDirection()` |
| `Capivara` | health | 30 | `touchPlayer(Player&)` |
| `Professor` | health | 80 | `shootBook(Player&)` |

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
