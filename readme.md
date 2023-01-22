# Traffic Simulator
This software is aimed to simulate traffic for a research paper that is due to the first week of February of 2023 using `C programming language` along with `SDL2 Library`. The structure of this software will contain the _traditional game loop_ where it has three phases: `listen`, `update` and `render`.

```c
while (true) {
    listen();
    update();
    render();
}
```

- `listen`: Listen for input in the keyboard/mouse, translate that input data to a instruction data (e.g. `COMMAND_QUIT`) and push that information to a queue inside the software.
- `update`: Process the said queue from `listen` one by one and do the instructions to mutate the game (a global variable). Remove the instruction from the queue after its been processed. Update the mandatory variables such as position of vehicles, timed cooldowns, etc.
- `render`: Display the game based on the information from a global variable that is mutated by `update`.

The said software is just simulation where a crossroad/single carriage-way intersection with vehicles moving from a source and destination where we control those behaviour of those vehicles using traffic lights. The goal is to implement an algorithm where those traffic lights automatically change and make it better than just manually controlling the traffic lights.
