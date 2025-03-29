# tinx
Temporal Inference Network eXecutor Suite 13.0.1, graphical shell 7.0.1

Design & coding by Andrea Giotti, 1998-1999, 2016-2025

TINX is a real time inference engine for system specification in an executable temporal logic. It is able to acquire, process and generate any binary, n-ary or real signal through POSIX
IPC, files or UNIX sockets. Specifications of signals and dynamic systems are represented as special graphs named temporal inference networks and executed in real time, with a
predictable sampling time which varies from few microseconds to some milliseconds, depending on the complexity of the specification.

Real time signal processing, dynamic system control, modeling of state machines, logical and mathematical property verification, realization of reactive systems are some fields of
application of this inference engine, which is deterministic but fully relational. It adopts driven forward reasoning in a three-valued logic, the clauses of which are assumed unknown
as default, to satisfy relations (2SAT) and it is able to run on an unlimited temporal horizon.

The accepted language is named Basic Temporal Logic and provides logical and mathematical operators, temporal operators on instants and intervals, parametrization of signals by
multidimensional arrays and bounded quantifiers on them.

This software runs on Linux operating system and it is distributed under GNU Public License.

