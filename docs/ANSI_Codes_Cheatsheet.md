# ANSI Codes Cheatsheet for Process Manager

## Text Colors (Foreground)

| Code | Color |
|------|-------|
| 30 | Black |
| 31 | Red |
| 32 | Green |
| 33 | Yellow |
| 34 | Blue |
| 35 | Magenta |
| 36 | Cyan |
| 37 | White |
| 90 | Bright Black (Dark Gray) |
| 91 | Bright Red |
| 92 | Bright Green |
| 93 | Bright Yellow |
| 94 | Bright Blue |
| 95 | Bright Magenta |
| 96 | Bright Cyan |
| 97 | Bright White |

## Background Colors

| Code | Color |
|------|-------|
| 40 | Black Background |
| 41 | Red Background |
| 42 | Green Background |
| 43 | Yellow Background |
| 44 | Blue Background |
| 45 | Magenta Background |
| 46 | Cyan Background |
| 47 | White Background |
| 100 | Bright Black Background |
| 101 | Bright Red Background |
| 102 | Bright Green Background |
| 103 | Bright Yellow Background |
| 104 | Bright Blue Background |

## Text Styles

| Code | Style |
|------|-------|
| 0 | Reset all |
| 1 | Bold |
| 2 | Dim |
| 4 | Underline |
| 7 | Reverse (Invert colors) |
| 8 | Hidden/Invisible |
| 9 | Strikethrough |

## Cursor Positioning & Formatting

| Command | Effect |
|---------|--------|
| `\x1B[H` | Move cursor to home (top-left) |
| `\x1B[2J` | Clear entire screen |
| `\x1B[K` | Clear from cursor to end of line |
| `\x1B[?25h` | Show cursor |
| `\x1B[?25l` | Hide cursor |
| `\x1B[{row};{col}H` | Move cursor to row, col (1-indexed) |

## Common Combinations

| Combination | Effect |
|-------------|--------|
| 31;1 | Red Bold |
| 32;1 | Green Bold |
| 33;1 | Yellow Bold |
| 92;1 | Bright Green Bold |
| 31;1;40 | Red Bold on Black |
| 33;44 | Yellow on Blue |
| 36;7 | Cyan Reverse |
| 37;2 | Dim White |

## Process Manager Color Scheme

| Status | Codes | Description |
|--------|-------|-------------|
| Running | 32;1 | Green Bold |
| Warning | 33;1 | Yellow Bold |
| Critical | 31;1;7 | Red Bold Reverse |
| Sleeping | 36;1 | Cyan Bold |
| Zombie/Stopped | 31;1 | Red Bold |
| Header | 1;37;40 | Bold White on Black |
| Metrics | 92;1 | Bright Green Bold |
| Low Priority | 90;2 | Dim Gray |
| Separator | 37;2 | Dim White |

## Layout Positioning Examples

| Position | Row | Column |
|----------|-----|--------|
| Header Row | 1 | 1 |
| System Metrics | 2-3 | 1 |
| Process List Start | 4 | 1 |
| Process Entry 1 | 5 | 1 |
| Process Entry N | 4+N | 1 |

## How Positioning Codes Work

ANSI positioning codes move your cursor to specific locations in the terminal, allowing you to control where text appears.

**Basic Syntax:**
```
\x1B[{row};{col}H
```
- `{row}` = vertical position (1-indexed, top = 1)
- `{col}` = horizontal position (1-indexed, left = 1)

**Concept:**
Think of your terminal as a grid where you can jump to any (row, column) position and print text from that point. This is how fixed layouts like top/htop work.

```
Col: 1    5    10   15   20
     ↓    ↓    ↓    ↓    ↓
R 1  ┌────────────────────┐
o    │ Cursor here (1,1)  │
w 2  │                    │
     │  Text at (2,5)     │
3    │                    │
     │    (5,10)          │
4    │                    │
     │                    │
5    └────────────────────┘
```

**Examples:**

| Command | Effect |
|---------|--------|
| `\x1B[1;1H` | Top-left corner (home) |
| `\x1B[2;1H` | Row 2, Column 1 (start of 2nd line) |
| `\x1B[5;10H` | Row 5, Column 10 (middle area) |
| `\x1B[10;1H` | Row 10, Column 1 (lower section) |

**Practical Process Manager Layout:**

```
\x1B[2J              → Clear screen
\x1B[1;1H            → Display header at top
\x1B[2;1H            → Display metrics
\x1B[4;1H            → Display column titles
\x1B[5;1H            → Display process 1
\x1B[6;1H            → Display process 2
\x1B[7;1H            → Display process 3
```

**Creating Fixed Displays:**

1. Clear screen with `\x1B[2J`
2. Position cursor at `(1,1)` and write header
3. Position cursor at `(2,1)` and write system metrics
4. Position cursor at `(4,1)` and write column titles
5. Loop through processes, incrementing row: `(5,1)`, `(6,1)`, `(7,1)`, etc.
6. Hide cursor with `\x1B[?25l` for clean appearance
7. On refresh, clear and redraw from scratch

**Key Difference:**
- Normal printing: sequential left-to-right, wraps to next line
- Positioning: jump to any (row, col), overwriting existing content

This is how you achieve the fixed display layout of process managers—you're painting specific regions of the screen independently.

## Usage Tips

- Combine multiple codes with semicolons: `\x1B[31;1;40m` (Red Bold on Black)
- Always end with `\x1B[0m` to reset formatting
- Use positioning codes to create fixed layouts like top/htop
- Alternating row backgrounds improve readability
- Use bright colors for headers, normal colors for content
- Reserve reverse (code 7) for critical alerts
