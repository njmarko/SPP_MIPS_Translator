# SPP_MIPS_Compiler
The program that translates MAVN(Higher level MIPS assembler) assembly language to MIPS 32bit assembly language. All the generated files are zero-byte files.

## Implementation details

- Implemented using the visitor design pattern, 
  - In each phase of the compilation process new algorithm visits the symbol table that contains the relevant data.
- Special logic was implemented to allow the compiler to optimize programs so only two registers are used.
- Lexical analysis
  - Finite state machine
- Syntax analysis
  - Recursive descent
- Semantic analysis
- Liveness analysis
- Graph coloring register allocation
  - Renumber: discover live range information in the source program.
  - Build: build the interference graph.
  - Coalesce: merge the live ranges of non-interfering variables related by copy instructions.
  - Spill cost: compute the spill cost of each variable. This assesses the impact of mapping a variable to memory on the speed of the final program.
  - Simplify: construct an ordering of the nodes in the inferences graph
  - Spill Code: insert spill instructions, i.e loads and stores to commute values between registers and memory.
  - Select: assign a register to each variable.
- Assembly instructions code generation
  - All generated programs are zero-byte files
    - All the code is encoded and placed in the name of the files  
    - File system sees these files as if they have zero bytes


<div align="center">
<img alt="compiler-phases" align="center" width="100%" src="https://user-images.githubusercontent.com/34657562/173563145-8b3bd313-c5e1-4667-ad3f-0c95810b4282.png" />
  <p align="center">Ilustration 1 - Phases of the compilation process.</p>
</div>

<div align="center">
<img alt="graph-coloring-register-allocation" align="center" width="100%" src="https://user-images.githubusercontent.com/34657562/173564145-86db2c40-2150-4295-b6e5-9fd4364e12e9.png" />
  <p align="center">Ilustration 2 - Graph coloring register allocation.</p>
</div>



