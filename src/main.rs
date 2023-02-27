pub const UCODE_HLT: u16 = 1 << 7;
pub const UCODE_MI: u16 = 1 << 6;
pub const UCODE_RI: u16 = 1 << 5;
pub const UCODE_RO: u16 = 1 << 4;
pub const UCODE_IO: u16 = 1 << 3;
pub const UCODE_II: u16 = 1 << 2;
pub const UCODE_AI: u16 = 1 << 1;
pub const UCODE_AO: u16 = 1 << 0;
pub const UCODE_EO: u16 = 1 << 15;
pub const UCODE_SU: u16 = 1 << 14;
pub const UCODE_BI: u16 = 1 << 13;
pub const UCODE_OI: u16 = 1 << 12;
pub const UCODE_CE: u16 = 1 << 11;
pub const UCODE_CO: u16 = 1 << 10;
pub const UCODE_J: u16 = 1 << 9;
pub const UCODE_FI: u16 = 1 << 8;

pub const INVERT: u16 = UCODE_MI | UCODE_RO | UCODE_IO | UCODE_II | UCODE_AI | UCODE_AO | UCODE_EO | UCODE_BI | UCODE_CO | UCODE_J | UCODE_FI;

pub const EE0: &'static [u8] = include_bytes!("/home/chen/arduino/microcode/ee0.bin");
pub const EE1: &'static [u8] = include_bytes!("/home/chen/arduino/microcode/ee1.bin");

#[derive(Default)]
pub struct Bus {
    value: Option<u8>,
}

#[derive(Default, Debug)]
pub struct Cpu {
    pub reg_a: u8,
    pub reg_b: u8,
    pub reg_i: u8,
    pub reg_m: u8,
    pub reg_c: u8,
    pub flag_c: bool,
    pub flag_z: bool,
    pub mem: [u8; 16],
}

impl Bus {
    pub fn get(&self) -> u8 {
        self.value.expect("attempt to use floating bus")
    }

    pub fn set(&mut self, value: u8) {
        if self.value.is_some() {
            panic!("attempt to enter locked bus");
        }

        self.value = Some(value);
    }
}

impl Cpu {
    pub fn ucode_step(&mut self, ucode: u16) {
        let mut bus = Bus::default();

        if ucode & UCODE_HLT != 0 {
            panic!("halt and catch fire");
        }

        if ucode & UCODE_RO != 0 {
            bus.set(self.mem[self.reg_m as usize]);
        }

        if ucode & UCODE_IO != 0 {
            bus.set(self.reg_i & 15);
        }

        if ucode & UCODE_AO != 0 {
            bus.set(self.reg_a);
        }

        if ucode & UCODE_EO != 0 {
            if ucode & UCODE_SU != 0 {
                bus.set(self.reg_a.wrapping_sub(self.reg_b));
            } else {
                bus.set(self.reg_a.wrapping_add(self.reg_b));
            }
        }

        if ucode & UCODE_CO != 0 {
            bus.set(self.reg_c);
        }

        if ucode & UCODE_MI != 0 {
            self.reg_m = bus.get();
        }

        if ucode & UCODE_RI != 0 {
            self.mem[self.reg_m as usize] = bus.get();
        }

        if ucode & UCODE_II != 0 {
            self.reg_i = bus.get();
        }

        if ucode & UCODE_AI != 0 {
            self.reg_a = bus.get();
        }

        if ucode & UCODE_BI != 0 {
            self.reg_b = bus.get();
        }

        if ucode & UCODE_OI != 0 {
            println!("out {}", bus.get());
        }

        if ucode & UCODE_CE != 0 {
            self.reg_c = self.reg_c.wrapping_add(1) & 15;
        }

        if ucode & UCODE_J != 0 {
            self.reg_c = bus.get();
        }

        if ucode & UCODE_FI != 0 {
            // panic!("FI not implemented");
        }
    }

    pub fn step(&mut self) {
        for i in 0..8 {
            let index = (self.flag_z as usize) << 9 | (self.flag_c as usize) << 8 | (self.reg_i as usize & 0xF0) >> 1 | i;
            let ucode = (EE1[index] as u16) << 8 | (EE0[index] as u16);
            self.ucode_step(ucode ^ INVERT);
        }
    }
}

fn main() {
    let mut vrisc = Cpu::default();

    vrisc.mem[0] = 0b0100_0001;
    vrisc.mem[1] = 0b0111_1111;
    vrisc.mem[2] = 0b0100_0000;
    vrisc.mem[3] = 0b0111_1110;
    vrisc.mem[4] = 0b0010_0000;
    vrisc.mem[5] = 0b1000_1111;
    vrisc.mem[6] = 0b0111_1111;
    vrisc.mem[7] = 0b0010_0000;
    vrisc.mem[8] = 0b1000_1110;
    vrisc.mem[9] = 0b0111_1110;
    vrisc.mem[10] = 0b1100_0100;

    for _ in 0..64 {
        vrisc.step();
    }
}
