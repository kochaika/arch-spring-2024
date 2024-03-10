use super::*;

#[test]
fn test_alu_addition() {
    let mut alu = ALU::new();
    assert_eq!(239 + 566, alu.perform_operation(239, 566, 32));
    assert!(!alu.get_zero_flag());
}

#[test]
fn test_alu_add_negative() {
    let mut alu = ALU::new();
    assert_eq!(5 + -3, alu.perform_operation(5, -3, 32));
    assert!(!alu.get_zero_flag());
}

#[test]
fn test_alu_add_zero() {
    let mut alu = ALU::new();
    assert_eq!(-2 + 2, alu.perform_operation(-2, 2, 32));
    assert!(alu.get_zero_flag());
}

#[test]
fn test_alu_overflow() {
    let mut alu = ALU::new();
    assert_eq!(i32::MIN, alu.perform_operation(i32::MAX, 1, 32));
    assert!(!alu.get_zero_flag());
}

#[test]
fn test_alu_sub() {
    let mut alu = ALU::new();
    assert_eq!(5 - 3, alu.perform_operation(5, 3, 34));
    assert!(!alu.get_zero_flag());
}

#[test]
fn test_alu_sub_neg() {
    let mut alu = ALU::new();
    assert_eq!(2 - 5, alu.perform_operation(2, 5, 34));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_sub_zero() {
    let mut alu = ALU::new();
    assert_eq!(5 - 5, alu.perform_operation(5, 5, 34));
    assert!(alu.get_zero_flag());
}

#[test]
fn alu_sub_overflow() {
    let mut alu = ALU::new();
    assert_eq!(i32::MAX, alu.perform_operation(i32::MIN, 1, 34));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_and_positive() {
    let mut alu = ALU::new();
    assert_eq!(7 & 5, alu.perform_operation(7, 5, 36));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_and_negative() {
    let mut alu = ALU::new();
    assert_eq!(-7 & -5, alu.perform_operation(-7, -5, 36));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_and_different_signs() {
    let mut alu = ALU::new();
    assert_eq!(-7 & 5, alu.perform_operation(-7, 5, 36));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_and_zero() {
    let mut alu = ALU::new();
    assert_eq!(2 & 4, alu.perform_operation(2, 4, 36));
    assert!(alu.get_zero_flag());
}

#[test]
fn alu_or_positive() {
    let mut alu = ALU::new();
    assert_eq!(7 | 5, alu.perform_operation(7, 5, 37));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_or_negative() {
    let mut alu = ALU::new();
    assert_eq!(-7 | -5, alu.perform_operation(-6, -5, 37));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_or_different_signs() {
    let mut alu = ALU::new();
    assert_eq!(-7 | 5, alu.perform_operation(-7, 5, 37));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_or_zero() {
    let mut alu = ALU::new();
    assert_eq!(0 | 0, alu.perform_operation(0, 0, 37));
    assert!(alu.get_zero_flag());
}

#[test]
fn alu_nor_positive() {
    let mut alu = ALU::new();
    assert_eq!(!(7 | 5), alu.perform_operation(7, 5, 39));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_nor_negative() {
    let mut alu = ALU::new();
    assert_eq!(!(-7 | -5), alu.perform_operation(-6, -5, 39));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_nor_different_signs() {
    let mut alu = ALU::new();
    assert_eq!(!(-7 | 5), alu.perform_operation(-7, 5, 39));
    assert!(!alu.get_zero_flag());
}

#[test]
fn alu_reset() {
    let mut alu = ALU::new();
    alu.perform_operation(0, 0, 32);
    assert!(alu.get_zero_flag());
    alu.reset();
    assert!(!alu.get_zero_flag());
}

#[test]
fn registers_default_zero() {
    let mut regs = Registers::new();
    for i in 0..32 {
        assert_eq!(0, regs.get_value(i));
    }
}

#[test]
#[should_panic]
fn registers_zero_set() {
    let mut regs = Registers::new();
    regs.set_value(0, 0x5051);
}

#[test]
#[should_panic]
fn registers_too_big() {
    let mut regs = Registers::new();
    regs.set_value(239, 1);
}

#[test]
fn registers_set() {
    let mut regs = Registers::new();
    for i in 1..32 {
        regs.set_value(i, 1 << i);
    }
    assert_eq!(0, regs.get_value(0));
    for i in 1..32 {
        assert_eq!(1 << i, regs.get_value(i));
    }
    for i in 1..32 {
        regs.set_value(i, 0);
    }
    for i in 1..32 {
        assert_eq!(0, regs.get_value(i));
    }
    for i in 31..1 {
        regs.set_value(i, 1 << i);
    }
    assert_eq!(0, regs.get_value(0));
    for i in 31..1 {
        assert_eq!(1 << i, regs.get_value(i));
    }
}

#[test]
fn registers_reset() {
    let mut regs = Registers::new();
    for i in 1..32 {
        regs.set_value(i, 1 << i);
    }
    assert_eq!(0, regs.get_value(0));
    for i in 1..32 {
        assert_eq!(1 << i, regs.get_value(i));
    }
    for i in 1..32 {
        regs.set_value(i, 0);
    }
    regs.reset();
    for i in 0..32 {
        assert_eq!(0, regs.get_value(i));
    }
}

#[test]
fn memory_initial() {
    let mut initial = Vec::new();
    initial.resize(MEMORY_SIZE, 0);

    let memory = Memory::new(initial.clone());
    for i in (0..MEMORY_SIZE).step_by(4) {
        assert_eq!(0, memory.get_word_from_position(i));
    }
    let num: i32 = 239;
    initial[0..4].copy_from_slice(&num.to_be_bytes());
    let memory = Memory::new(initial.clone());
    for i in (0..MEMORY_SIZE).step_by(4) {
        assert_eq!(if i == 0 { 239 } else { 0 }, memory.get_word_from_position(i));
    }
}

#[test]
fn test_set() {
    let mut memory = Memory::new(Vec::new());
    memory.set_word_at_position(0, -1);
    assert_eq!(-1, memory.get_word_from_position(0));
    assert_eq!(-256, memory.get_word_from_position(1)); // overlapping works fine
}

#[test]
fn memory_reset() {
    let mut inital_memory = Memory::new(Vec::new());
    for i in (0..MEMORY_SIZE).step_by(4) {
        assert_eq!(0, inital_memory.get_word_from_position(i));
    }
    for i in (0..MEMORY_SIZE).step_by(4) {
        inital_memory.set_word_at_position(i, -1);
    }
    for i in (0..MEMORY_SIZE).step_by(4) {
        assert_eq!(-1, inital_memory.get_word_from_position(i));
    }
    inital_memory.reset();
    for i in (0..MEMORY_SIZE).step_by(4) {
        assert_eq!(0, inital_memory.get_word_from_position(i));
    }
}

fn test_fetch_decode(fsm: &mut FSM, opcode: u8, funct: u8) {
    assert_eq!(FSMState::Fetch, fsm.current_state);
    let fetch = fsm.get_decision();
    assert_eq!(false, fetch.iord);
    assert_eq!(false, fetch.alu_src_a_reg);
    assert_eq!(1, fetch.alu_source_b);
    assert_eq!(32, fetch.alu_control);
    assert_eq!(0, fetch.pc_source);
    assert_eq!(true, fetch.ir_write);
    assert_eq!(true, fetch.pc_write);
    fsm.set_instruction(opcode, funct);

    assert_eq!(FSMState::Decode, fsm.current_state);
    let decode = fsm.get_decision();
    assert_eq!(false, decode.alu_src_a_reg);
    assert_eq!(3, decode.alu_source_b);
    assert_eq!(32, decode.alu_control);
}

#[test]
fn fsm_j_type() {
    let mut fsm = FSM::new();
    test_fetch_decode(&mut fsm, 2, 0);
    assert_eq!(FSMState::JType, fsm.current_state);
    let j = fsm.get_decision();
    assert_eq!(2, j.pc_source);
    assert_eq!(true, j.pc_write);
    assert_eq!(FSMState::Fetch, fsm.current_state);
}

#[test]
fn fsm_beq() {
    let mut fsm = FSM::new();
    test_fetch_decode(&mut fsm, 4, 0);
    assert_eq!(FSMState::Branch, fsm.current_state);
    let b = fsm.get_decision();
    assert_eq!(true, b.alu_src_a_reg);
    assert_eq!(0, b.alu_source_b);
    assert_eq!(34, b.alu_control);
    assert_eq!(1, b.pc_source);
    assert_eq!(true, b.branch);
    assert_eq!(false, b.negate_zero);

    assert_eq!(FSMState::Fetch, fsm.current_state);
}

#[test]
fn fsm_bne() {
    let mut fsm = FSM::new();
    test_fetch_decode(&mut fsm, 5, 0);
    assert_eq!(FSMState::Branch, fsm.current_state);
    let b = fsm.get_decision();
    assert_eq!(true, b.alu_src_a_reg);
    assert_eq!(0, b.alu_source_b);
    assert_eq!(34, b.alu_control);
    assert_eq!(1, b.pc_source);
    assert_eq!(true, b.branch);
    assert_eq!(true, b.negate_zero);

    assert_eq!(FSMState::Fetch, fsm.current_state);
}

#[test]
fn fsm_sw() {
    let mut fsm = FSM::new();
    test_fetch_decode(&mut fsm, 43, 0);
    assert_eq!(FSMState::ITypeAddressCompute, fsm.current_state);
    let memory_compute = fsm.get_decision();
    assert_eq!(true, memory_compute.alu_src_a_reg);
    assert_eq!(2, memory_compute.alu_source_b);
    assert_eq!(32, memory_compute.alu_control);

    assert_eq!(FSMState::ITypeMemoryWrite, fsm.current_state);
    let memory_write = fsm.get_decision();
    assert_eq!(true, memory_write.iord);
    assert_eq!(true, memory_write.mem_write);

    assert_eq!(FSMState::Fetch, fsm.current_state);
}

#[test]
fn fsm_lw() {
    let mut fsm = FSM::new();
    test_fetch_decode(&mut fsm, 34, 0);
    assert_eq!(FSMState::ITypeAddressCompute, fsm.current_state);
    let memory_compute = fsm.get_decision();
    assert_eq!(true, memory_compute.alu_src_a_reg);
    assert_eq!(2, memory_compute.alu_source_b);
    assert_eq!(32, memory_compute.alu_control);

    assert_eq!(FSMState::ITypeMemoryRead, fsm.current_state);
    let memory_read = fsm.get_decision();
    assert_eq!(true, memory_read.iord);

    assert_eq!(FSMState::ITypeReadWriteback, fsm.current_state);
    let memory_writeback = fsm.get_decision();
    assert_eq!(false, memory_writeback.reg_dst);
    assert_eq!(true, memory_writeback.mem_to_reg);
    assert_eq!(true, memory_writeback.reg_write);

    assert_eq!(FSMState::Fetch, fsm.current_state);
}

#[test]
fn fsm_r_type() {
    let mut fsm = FSM::new();
    for funct in [32u8, 34, 36, 37, 39] {
        test_fetch_decode(&mut fsm, 0, funct);

        assert_eq!(FSMState::RTypeExecute, fsm.current_state);
        let execute = fsm.get_decision();
        assert_eq!(true, execute.alu_src_a_reg);
        assert_eq!(0, execute.alu_source_b);
        assert_eq!(funct, execute.alu_control);

        assert_eq!(FSMState::RTypeALUWriteBack, fsm.current_state);
        let writeback = fsm.get_decision();
        assert_eq!(true, writeback.reg_dst);
        assert_eq!(false, writeback.mem_to_reg);
        assert_eq!(true, writeback.reg_write);
    }
}
