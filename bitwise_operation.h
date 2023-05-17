#define set_bit(Reg, bit)   ( Reg |= (1<<bit))
#define clear_bit(Reg, bit)   (Reg &= ~(1<<bit))
#define get_bit(Reg, bit)   (Reg & (1<<bit))
#define toggle_bit(Reg,  bit)   (Reg ^= (1<<bit))
