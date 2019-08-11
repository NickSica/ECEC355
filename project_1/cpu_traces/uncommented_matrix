addi x18, x0, 4
addi x19, x0, 0
beq x19, x18, 120
addi x20, x0, 0
slli x29, x19, 3
add x29, x29, x11
sd x0, 0(x29)
beq x20, x18, 92
slli x7, x19, 2
add x7, x7, x20
slli x7, x7, 3
add x7, x7, x10
ld x7, 0(x7)
sd x10, 0(x2)
sd x11, -8(x2)
sd x1, -16(x2)
addi x2, x2, -24
addi x10, x7, 0
addi x11, x20, 0
jal x1, 56
addi x7, x10, 0
addi x2, x2, 24
ld x10, 0(x2)
ld x11, -8(x2)
ld x1, -16(x2)
ld x30, 0(x29)
add x30, x30, x7
sd x30, 0(x29)
addi x20, x20, 1
beq x0, x0, -88
addi x19, x19, 1
beq x0, x0, -116
jalr x1, 0(x1)
sll x10, x10, x11
jalr x1, 0(x1)