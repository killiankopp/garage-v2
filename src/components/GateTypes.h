#ifndef GATE_TYPES_H
#define GATE_TYPES_H

// Possible gate states
enum GateState {
  CLOSED,
  OPEN,
  UNKNOWN
};

// Operation states for timeout monitoring
enum OperationState {
  IDLE,
  OPENING,
  CLOSING
};

#endif // GATE_TYPES_H