# Digging Jim

![Main Menu](/docs/images/main_menu.png)

This project is a fan-made re-creation of the game Digging Jim, a Boulder Dash–style game originally developed and published by Persei Entertainment in 1999 for Windows.

The original game no longer runs well on modern hardware, so this remake was created as a non-commercial tribute. It is not affiliated with or endorsed by Persei Entertainment. Please support the official release where possible.

## Credits

Original Game (1999) by Persei Entertainment
* Programming: **Peter Præst**
* Graphics: **Robert Kjettrup**
* Sound: **Henrik Sundberg**, **Peter Præst**
* Cave Design: **Robert Kjettrup**, **Peter Præst**, **Anders Hansen**

Remake (2025)
* Recreation: **Christopher Malcolm**

![Screenshots](/docs/images/screenshots.png)

---

## Gameplay

The player will have control of Jim.

The goal is to collect a certain amount of diamonds in the cave, to unlock and leave through the exit door.

The amount of diamonds required is called the diamond quota.

Caves will contain obstacles and enemies in an attempt to thwart Jim's attempt at completing each cave.

There is also a cave timer, which if it gets to 0 seconds, will cause Jim to self-destruct.

There are 100 caves in the original game.

The original game also had a Builder (coming soon) where players could build their own cave files. Some examples of these have been added to this project as well.

![Gameplay](/docs/images/gameplay.png)

---

## Controls

Keyboard controls have remained faithful to the original game. The original also supported joystick, so support for controllers and joysticks have been added in as well.

| Keyboard | Controller/Joystick | Action |  Description |
| - | - | - | - |
| `Arrow Keys`/`W` `A` `S` `D` | `Analog Stick` | Move/Navigate | Control Jim's movement when traversing the cave environment. You can also move pushable objects such as boulders by holding against it for 1 second. Also used to navigate the Main Menu. |
| `Enter` | `A` / `Cross` / `Button 0` | Select/Confirm | Select and confirm options in the Main Menu. Also used to restart a cave if Jim is defeated.  |
| `Space` | `X` / `Square` / `Button 2` | Collect mode | Collect mode allows Jim to dig in an adjacent space without actually moving there. This can be used to collect adjacent diamonds or release enemies without touching them. |
| `Tab` | `B` / `Circle` / `Button 1` | Self-destruct | Sometimes Jim will become trapped and unable to complete the cave. If that is the case, you can use the self-destruct to restart the cave, before the timer runs out. |
| `Esc` | `Back` / `Select` / `Button 6` | Quit | Exit the cave and navigate back to the Main Menu. |
| `P` | `Start` | Pause/Resume | Toggle pausing when playing a cave. |

---

## Cheat mode

The original game also had a "cheat" mode. This was activated by pressing the `F12` key. Once in cheat mode, pressing `F1` would let you skip forward caves in the game, and also allowed the player in increment caves by 1 instead of 5 in the Main Menu.

The same functionality is present here, with the cheat activation button being mapped to `F11` instead.

Additional cheat buttons have also been added to the game, which are listed below.
 
`F1` – Go to next cave  
`F2` – Restart current cave  
`F3` – Go to previous cave  
`F4` – Open Cave Properties window  
`F5` – Toggle Editor Panel  
`F6` – Move camera target left  
`F7` – Move camera target up  
`F8` – Move camera target down  
`F9` – Move camera target right  
`F10` – Reset camera target to Jim  
`F11` – Activate cheat mode  

### Cave Properties window

This is the Cave Properties window. It can be used to edit various properties of the cave, such as diamond quota and the colour. These changes will be applied in real-time. If the cave reloads, these properties will be reset back to the original.

![Cave Properties](/docs/images/cave_properties.png)

### Editor Panel
This is the Editor Panel. Use it to add cave entities into the cave by using the mouse.

![Editor Panel](/docs/images/editor_panel.png)

---

## Cave File Format

A cave file is a document containing data for Digging Jim caves. These files have the extension `.cav` and have a particular byte format.

Below is a diagram of this format, each row represents **8 bytes**. Offsets are shown in hex on the left.

```text
0x00 | C   A   V   E   c0  c1  c2  c3| -- Header
0x08 | *   *   *   *   *   *   *   * | * First 4 bytes are literally the characters "C", "A", "V", "E"
0x10 | *   *   *   *   *   *   *   * | * Following 4 bytes (c0-c3) represent the number of caves in the file as a uint32
0x18 | *   *   *   *   *   *   *   * | * The remaining 80 bytes are all padding
0x20 | *   *   *   *   *   *   *   * |
0x28 | *   *   *   *   *   *   *   * |
0x30 | *   *   *   *   *   *   *   * |
0x38 | *   *   *   *   *   *   *   * |
0x40 | *   *   *   *   *   *   *   * |
0x48 | *   *   *   *   *   *   *   * |
0x50 | *   *   *   *   *   *   *   * |
0x58 | w0  w1  h0  h1  p0  p1  p2  p3| -- Payload
0x60 | t0  t1  t2  t3  q0  q1  q2  q3| * First 4 bytes (w0-w1, h0-h1) represent the dimensions of the cave (width, height) as uint16.
0x68 | v0  v1  v2  v3  e0  e1  e2  e3|   Defaults of width = 50 and height == 30 are used value is 0 (for the original caves, this was always 0)
0x70 | a0  a1  a2  a3  m0  m1  m2  m3| * Following 4 bytes (p0-p3) represent the plasma growth speed as a uint32 (1-1000)
0x78 | M0  M1  M2  M3  H0  H1  H2  H3| * Next 4 bytes (t0-t3) represent the starting cave timer value as a uint32 (0-999)
0x80 | s0  s1  s2  s3  l0  l1  l2  l3| * Next 4 bytes (q0-q3) represent the diamond quota as a uint32 (0-999)
0x88 | d   d   d   d   d   d   d   d | * Next 4 bytes (v0-v3) represent the diamond value as a uint32 (0-99)
0x90 | d   d   d   d   d   d   d   d | * Next 4 bytes (e0-e3) represent the extra diamond value (0-99)
0x98 | d   d   d   d   d   d   d   d | * Next 4 bytes (a0-a3) represent the amoeba growth speed as a uint32 (1-1000)
0xA0 | d   d   d   d   d   d   d   d | * Next 4 bytes (m0-m3) represent the amoeba growth max
0xA8 | d   d   d   d   d   d   d   d | * Next 4 bytes (M0-M3) represent the magic wall expiry time (0-999)
0xB0 | d   d   d   d   d   d   d   d | * Next 4 bytes (H0-H3) represent the cave colour hue (0-200)
0xB8 | d   d   d   d   d   d   d   d | * Next 4 bytes (s0-s3) represent the cave colour saturation (0-200)
0xC0 | d   d   d   d   d   d   d   d | * Next 4 bytes (l0-l3) represent the cave colour luminance (0-200)
0xC8 | d   d   d   d   d   d   d   d | 
0xD0 | d   d   d   d   d   d   d   d | * Remaining data for the cave will be the next (width x height) bytes
0xD8 | d   d   d   d   d   d   d   d |   This is the map grid where each byte represents a cave entity
0xE0 | d   d   d   d   d   d   d   d | 
0xE8 | d   d   d   d   d   d   d   d | * Once data for cave end, data for next cave will begin, with the cave properties
0xF0 | d   d   d   d   d   d   d   d |   This will repeat for each cave until the cave count is exhausted.
0xF8 | d   d   d   d   d   d   d   d |
...
```