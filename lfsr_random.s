.data

lfsr:
        .align 4
        .half
        0x1

.text

# Implements a 16-bit lfsr
#
# Arguments: None


# $t0 : reg (updates after each iteration)
# $t1 : int i (counter)
# $v0 : result
# $t2 : ends as (highest << 15)
# $t3 : ends as (reg >> 1)
# $t4 : changes with each XOR


lfsr_random:

        la $t0 lfsr
        lhu $v0 0($t0)

        # int i;
        li $t1, 0

        for_loop:

            la $t0, 0($v0)

            # if i == 16;
        	beq $t1, 16, for_loop_done

            # t2 = reg >> 0
            srl $t2, $t0, 0

            # t4 = reg >> 2
            srl $t4, $t0, 2 

            # t2 = (reg >> 0) ^ (reg >> 2)
            xor $t2, $t2, $t4

            # t4 = reg >> 3
            srl $t4, $t0, 3

            # t2 = ((reg >> 0) ^ (reg >> 2)) ^ (reg >> 3)
            xor $t2, $t2, $t4

            # t4 = (reg >> 5)
            srl $t4, $t0, 5          

            # t2 = highest = ((reg >> 0) ^ (reg >> 2)) ^ (reg >> 3)) ^ (reg >> 5)
            xor $t2, $t2, $t4

            # t3 = (reg >> 1)
            srl $t3, $t0, 1

            # t2 = (highest << 15)
            sll $t2, $t2, 15

            # make half-words
            andi $t2, $t2, 0xFFFF
            andi $t3, $t3, 0xFFFF

            # $v0 = (reg >> 1) | (highest << 15)
            or $v0, $t3, $t2

        	# i++;
        	addi $t1, $t1, 1
        	j for_loop

        for_loop_done:

        li $t1, 0

        # Don't change these lines
        la $t0 lfsr
        sh $v0 0($t0)
        jr $ra
