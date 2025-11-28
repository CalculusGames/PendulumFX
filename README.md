# PendulumFX

> Advanced multi-pendulum physics simulation powered by cmdfx

A real-time terminal-based pendulum simulation that supports single and multi-pendulum systems with accurate physics modeling, including precise double-pendulum equations and approximated multi-body dynamics for chains of 3+ pendulums.

## Features

- **Interactive & Non-Interactive Modes**: Run with full CLI control or interactive prompts
- **Multi-Pendulum Support**: Simulate chains of interconnected pendulums (1 to N pendulums)
- **Accurate Physics**: Implements proper double-pendulum equations and multi-body approximations
- **Customizable Parameters**: Control gravity, damping, coupling, colors, lengths, and masses
- **Visual Trails**: Optional trail drawing for complex motion visualization
- **Real-time Animation**: Smooth 60fps terminal graphics with configurable time multiplier

## Building the Application

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- cmdfx library

### Build Instructions

```bash
# Clone and navigate to the project
cd PendulumFX

# Create build directory and configure
mkdir -p build
cd build
cmake ..

# Build the application
cmake --build .

# Run the executable
./bin/PendulumFX
```

## Usage

### Interactive Mode (Default)

Run without arguments for interactive setup:

```bash
./bin/PendulumFX
```

### Non-Interactive Mode

Use CLI arguments for automated execution:

```bash
./bin/PendulumFX --no-interactive [options]
```

## CLI Arguments

| Argument | Short | Description | Default |
|----------|-------|-------------|---------|
| `--no-interactive` | `--noint` | Skip interactive prompts | Interactive mode |
| `--count` | `-c` | Number of pendulums | 2 (in non-interactive) |
| `--gravity` | `-g` | Gravitational acceleration | 9.81 |
| `--multiplier` | `-m` | Time simulation multiplier | 1.0 |
| `--damping` | | Global damping coefficient | 0.01 |
| `--coupling` | | Multi-pendulum coupling strength | 0.5 |
| `--draw-trails` | `-t` | Enable motion trails | false |
| `--color` | | Pendulum color (hex format) | Auto-generated |
| `--length` | `-l` | Pendulum length (can use multiple) | Auto-calculated |
| `--mass` | `-ms` | Pendulum mass (can use multiple) | Auto-calculated |

## Examples

### Single Pendulum

```bash
# Simple single pendulum with default settings
./bin/PendulumFX --no-interactive --count 1

# Single pendulum with custom parameters
./bin/PendulumFX --no-interactive --count 1 --length 15.0 --mass 2.0 --gravity 5.0 --color FF0000
```

### Double Pendulum (Chaotic System)

```bash
# Classic double pendulum with precise physics
./bin/PendulumFX --no-interactive --count 2

# Double pendulum with trails and custom colors
./bin/PendulumFX --no-interactive --count 2 --draw-trails \
  --color FF0000 --color 00FF00 \
  --length 12.0 --length 8.0 \
  --mass 1.5 --mass 1.0

# Slow-motion double pendulum on the moon
./bin/PendulumFX --no-interactive --count 2 --gravity 1.62 --multiplier 0.5
```

### Multi-Pendulum Chain (5 Pendulums)

```bash
# Five-pendulum chain with default settings
./bin/PendulumFX --no-interactive --count 5

# Custom five-pendulum setup with rainbow colors
./bin/PendulumFX --no-interactive --count 5 --draw-trails \
  --color FF0000 --color FF8000 --color FFFF00 --color 00FF00 --color 0000FF \
  --length 10.0 --length 8.0 --length 6.0 --length 4.0 --length 2.0

# High-energy system with strong coupling
./bin/PendulumFX --no-interactive --count 5 --coupling 1.5 --damping 0.005 --multiplier 2.0
```

### Advanced Examples

```bash
# Zero gravity pendulum (space simulation)
./bin/PendulumFX --no-interactive --count 3 --gravity 0.1 --damping 0.001

# Heavy damping (underwater simulation)
./bin/PendulumFX --no-interactive --count 2 --damping 0.1 --multiplier 0.3

# Jupiter gravity with massive pendulums
./bin/PendulumFX --no-interactive --count 4 --gravity 24.79 \
  --mass 5.0 --mass 4.0 --mass 3.0 --mass 2.0

# Micro-pendulum precision demo
./bin/PendulumFX --no-interactive --count 3 \
  --length 3.0 --length 2.0 --length 1.0 \
  --multiplier 0.1 --coupling 0.1 --draw-trails
```

## Technical Implementation

### Single Pendulum Physics

For a single pendulum, the equation of motion is derived from the Lagrangian mechanics:

$$\ddot{\theta} = -\frac{g}{l}\sin(\theta) - d\dot{\theta}$$

Where:

- $\theta$ is the angle from vertical
- $g$ is gravitational acceleration
- $l$ is pendulum length
- $d$ is the damping coefficient

### Double Pendulum Physics

For exactly two pendulums, PendulumFX implements the precise coupled differential equations. The system is governed by:

$$\ddot{\theta_1} = \frac{-g(2m_1 + m_2)\sin(\theta_1) - m_2 g \sin(\theta_1 - 2\theta_2) - 2\sin(\theta_1 - \theta_2)m_2[\dot{\theta_2}^2 l_2 + \dot{\theta_1}^2 l_1 \cos(\theta_1 - \theta_2)]}{l_1[2m_1 + m_2 - m_2\cos(2(\theta_1 - \theta_2))]}$$

$$\ddot{\theta_2} = \frac{2\sin(\theta_1 - \theta_2)\left[\dot{\theta_1}^2 l_1(m_1 + m_2) + g(m_1 + m_2)\cos(\theta_1) + \dot{\theta_2}^2 l_2 m_2 \cos(\theta_1 - \theta_2)\right]}{l_2\left[2m_1 + m_2 - m_2\cos(2(\theta_1 - \theta_2))\right]}$$

Where:

- $m_1, m_2$ are the masses
- $l_1, l_2$ are the lengths
- $\theta_1, \theta_2$ are the angles from vertical

### Multi-Pendulum Lagrangian Dynamics (N ≥ 3)

For chains of three or more pendulums, PendulumFX implements a Lagrangian-based multi-body system that captures the complex constraint forces between interconnected segments. The system accounts for:

1. **Gravitational Torque**: $-\frac{g}{l_i}\sin(\theta_i)$ for each pendulum
2. **Constraint Forces**: Lagrangian constraint forces from suspended masses
3. **Multi-Body Coupling**: Mass-weighted interactions across the entire chain
4. **Nearest-Neighbor Effects**: Direct coupling between adjacent pendulums

The acceleration for pendulum $i$ in a multi-body chain is:

$$\ddot{\theta_i} = -\frac{g}{l_i}\sin(\theta_i) + F_{constraint}(i) + F_{adjacent}(i)$$

Where the constraint force accounts for all suspended masses:

$$F_{constraint}(i) = \sum_{j=i+1}^{n} \frac{m_j}{m_i + M_{below}} \cdot \frac{l_j}{l_i \cdot d_{ij}^2} \cdot \left[\dot{\theta_j}^2 \sin(\theta_j - \theta_i) + \frac{c_{global}}{2} \sin(\theta_j - \theta_i)\right]$$

And the adjacent coupling force is:

$$F_{adjacent}(i) = \sum_{k \in \{i-1,i+1\}} w_k \cdot \frac{m_k}{m_i + m_k} \cdot \frac{l_k}{l_i} \cdot \dot{\theta_k}^2 \sin(\theta_k - \theta_i)$$

Where:

- $M_{below} = \sum_{j=i+1}^{n} m_j$ is the total suspended mass
- $d_{ij}$ is the normalized path distance from pendulum $i$ to $j$
- $w_k$ are coupling weights (0.3 for parent, 0.15 for child)
- $c_{global}$ is the global coupling parameter

This formulation preserves energy conservation principles while maintaining computational efficiency for real-time simulation.

### Numerical Integration

The simulation uses explicit Euler integration with adaptive timestep:

```cpp
angularVelocity += angularAcceleration * timeStep;
angularVelocity *= (1.0 - damping * timeStep);  // Apply damping
angle += angularVelocity * timeStep;
```

### Rendering Pipeline

The visualization system employs a double-buffered approach:

1. **Erase Phase**: Clear previous pendulum positions using space characters
2. **Update Phase**: Compute new physics state for all pendulums
3. **Draw Phase**: Render new positions with colored characters (`+` for rods, `@` for masses)
4. **Trail Management**: Optionally preserve ball positions for motion trails

The rendering maintains smooth 60fps animation by sleeping for approximately 16ms between frames, with the actual physics timestep scaled by the time multiplier.

## Controls

- **Ctrl+C**: Exit the simulation
- **Interactive Mode**: Follow on-screen prompts for parameter configuration
- **Non-Interactive Mode**: All parameters specified via command line

## Performance Notes

- **Double Pendulum**: Uses exact equations for maximum accuracy
- **Multi-Pendulum**: Approximated dynamics optimized for real-time performance
- **Frame Rate**: Locked to ~60fps for smooth animation
- **Memory Usage**: Minimal - stores only current and previous states

The simulation balances physical accuracy with computational efficiency, providing visually compelling results while maintaining interactive frame rates even for complex multi-pendulum systems.
