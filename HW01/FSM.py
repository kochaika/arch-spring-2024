class FSM:
    def __init__(self, states):
        self.states = states
        self.current_state = None

    def set_state(self, state):
        if state in self.states:
            self.current_state = state
        else:
            raise ValueError("Invalid state")

    def execute_state(self):
        # temporary impl
        if self.current_state is not None:
            if self.current_state == "FETCH":
                print("Fetching instruction from Program Memory")
            elif self.current_state == "DECODE":
                print("Decoding instruction")
            elif self.current_state == "EXECUTE":
                print("Executing instruction")
        else:
            raise ValueError("Current state not set")
