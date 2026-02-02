import re
import matplotlib.pyplot as plt

# AI generated code to plot sda/sck signals over time
# paste entire monitor output as data to plot sda/sck
data = """
hello
104
sck = 1, sda = 1
sck = 1, sda = 1
init
sck = 0, sda = 1
sck = 0, sda = 1
sck = 0, sda = 1
Starting
sck = 0, sda = 1
sck = 0, sda = 1
sck = 0, sda = 1
sck = 0, sda = 1
stop
sck = 0, sda = 1
sck = 0, sda = 1
sck = 1, sda = 1
result = 64
temp = 36.718235
"""

sck_vals = []
sda_vals = []

pattern = re.compile(r"sck\s*=\s*(\d+),\s*sda\s*=\s*(\d+)")

for line in data.splitlines():
    match = pattern.search(line)
    if match:
        sck_vals.append(int(match.group(1)))
        sda_vals.append(int(match.group(2)))

# Build time axis (just index)
t = list(range(len(sck_vals)))

plt.figure(figsize=(12, 5))

plt.step(t, sck_vals, where="post", label="SCK")
plt.step(t, sda_vals, where="post", label="SDA")

plt.ylim(-0.5, 1.5)
plt.yticks([0, 1])
plt.xlabel("Sample")
plt.ylabel("Level")
plt.title("SCK / SDA Waveform")
plt.legend()
plt.grid(True)

plt.show()

