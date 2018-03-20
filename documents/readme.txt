
TINX 4.9 Suite

A real time inference engine for temporal logic specifications, which is able to process and generate any binary signal through POSIX IPC or files. Specifications of signals are represented as special graphs and executed in real time, with a sampling time of few milliseconds. The accepted language provides timed logic operators, conditional operators, interval operators, bounded quantifiers and parametrization of signals.

Unique in its genre, this software provides low level parallelism in real time when executing temporal logic specifications. Its temporal logic compiler is flexible and optimizing. Automatic scheduling of timed actions inferred from logic specifications without explicit programming is an effective way to reproduce the behaviour of binary real time systems.

This distribution includes a very fast, single-threaded version (4.3) and a multithreaded version (4.3 MT) of the inference engine (Temporal Inference Network eXecutor). For an explanation of the executed networks, see the degree thesis included in the documentation (Italian only). These networks can represent general temporal logic specifications of real time systems and the inference engine can process and generate binary signals in real time according to these specifications, with a sampling time of the order of few milliseconds on a modern hardware.

An open source compiler (Temporal Inference Network Generator) is included, to generate these networks from a temporal logic source file, together with a graphical shell (Graphical TINX SHell) for signal generation and display which shows how to control and interface the inference engine. They allow the practical use of this tool in the context of a development system for general temporal logic specifications. TINX 1.0 is not a real time application but can be compiled and run on any ANSI C compliant environment, the other versions are real time applications for Linux.

This software is licensed under the GNU Public License and includes the executor itself, the compiler, a graphical shell and some examples of temporal logic specifications of simple dynamical systems.

Prof. Andrea Giotti, PhD

