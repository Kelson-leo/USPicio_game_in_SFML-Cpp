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
| `Drawable` | `IRenderer.h` | Interface abstrata: `void draw(IRenderer&) const` |
| `KeyCode` | `IInputHandler.h` | Enum: A-Z, Space, Enter, Escape, Left, Right, Up, Down |
| `EventType` | `IInputHandler.h` | Enum: Closed, KeyPressed, KeyReleased |
| `Event` | `IInputHandler.h` | `{EventType type; KeyCode key;}` |

A tradução Core ↔ SFML fica em `src/infrastructure/SfmlConversions.h` (header-only, funções inline `toSfml()` / `toCore()`).

## 5. Constantes Físicas

Definidas em `src/core/PhysicsConstants.h` (resolução de referência: 1080p):

| Constante | Valor | Unidade |
|---|---|---|
| `GROUND_Y` | 880.0f | pixels (topo do tile de chão) |
| `GROUND_HEIGHT` | 200.0f | pixels |
| `GRAVITY` | 800.0f | pixels/s² |
| `JUMP_SPEED` | -400.0f | pixels/s (negativo = para cima) |

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

### Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cd build && ctest
```

### Commit
- Padrão: `[Sprint N] - Descrição clara em inglês`
- Exemplo: `[Sprint 1] - Add fixed timestep game loop and menu placeholder`

## 8. Fluxo de Comunicação

1. Toda sessão começa relendo `CLAUDE.md`.
2. Antes de implementar qualquer Sprint, entregar **Review Técnico** ao PO.
3. Só codificar após autorização explícita ("Aprovado").
4. Atualizar `CLAUDE.md` sempre que novas regras ou decisões arquiteturais forem tomadas.

---

## 9. Débitos Técnicos Conhecidos

| ID | Descrição | Impacto | Resolução planejada |
|---|---|---|---|
| TD-01 | `Game::render()` faz `dynamic_cast<SfmlRenderer&>` para desenhar `sf::Drawable` direto | `gameplay/` acoplado a `infrastructure/` | Sprint 2: criar game-objects que implementam `core::Drawable` |
| TD-02 | `SfmlInput` armazena `sf::RenderWindow&` (não usa porta `IRenderer`) | Acoplamento direto ao SFML no adapter de input | Revisar na Sprint 2 junto com TD-01 |

---

## Histórico de Sprints

| Sprint | Data | Descrição |
|---|---|---|
| 0 | 2026-06-19 | Setup inicial: CMake, estrutura de diretórios, ports, adapters, Game loop, menu placeholder |
| 1.1 | 2026-06-19 | Refatoração arquitetural: remoção de SFML do Core, tipos próprios, ITextureLoader, PhysicsConstants, SfmlConversions, fonte PressStart2P.ttf, 11 testes passando |
