.text

# Generates an autostereogram inside of buffer
#
# Arguments:
#     autostereogram (unsigned char*)
#     depth_map (unsigned char*)
#     width
#     height
#     strip_size

# .include "lfsr_random.s"

calc_autostereogram:

        # Allocate 5 spaces for $s0-$s5
        # (add more if necessary)
        addiu $sp $sp -32
        sw $s0 0($sp)
        sw $s1 4($sp)
        sw $s2 8($sp)
        sw $s3 12($sp)
        sw $s4 16($sp)

        # Create space for necessary variables i, j, ra
        sw $t1 20($sp)              # t1 = i
        sw $t2 24($sp)              # t2 = j
        sw $ra 28($sp)              # ra = ra

        # autostereogram
        lw $s0 32($sp)
        # depth_map
        lw $s1 36($sp)
        # width
        lw $s2 40($sp)
        # height
        lw $s3 44($sp)
        # strip_size
        lw $s4 48($sp)


        li $t1, 0       # int i = 0
        first_for_loop:

        # if i == M, go to end
        beq $t1, $s2, end

                li $t2, 0          # int j = 0;
                second_for_loop:

                beq $t2, $s3, second_for_loop_done

                    # if i == M, go to end
                    beq $t1, $s2, end

                    # if j == N, go to second_for_loop_done
                    beq $t2, $s3, second_for_loop_done

                    # if i < S, go to LFSR_CALC
                    slt $t3, $t1, $s4
                    bne $t3, $0, LFSR_CALC

                    #ELSE_CLAUSE 
                        #CALCULATING i + depth(i , j) - S
                            #Access depth_map[i, j]
                                mulu $t5, $t2, $s2          # $t5 = j * width
                                addu $t5, $t5, $t1          # $t5 = t5 + i
                                addu $t5, $s1, $t5          # t5 = depth[i, j]

                            #Arithmetic
                                lb $t6, 0($t5)              # $t6 = depth[i, j]
                                addu $t6, $t6, $t1          # $t6 = t6 + i
                                sub $t6, $t6, $s4           # $t6 = t6 - S

                        #ACCESS I(i + depth(i,j) - S, j)

                            #Calculate depth_map[i, j]
                                mulu $t4, $t2, $s2      # $t4 = j * width
                                addu $t4, $t4, $t6      # $t4 = t4 + i
                                addu $t4, $t4, $s0      # $t4 = ADDRESS(depth_map[i, j])
                                lb $t6, 0($t4)          # $t6 = depth_map[i, j]

                            #Calculate I[i, j]
                                mulu $t7, $t2, $s2      # $t7 = j * width
                                addu $t7, $t1, $t7      # $t7 = t7 + i
                                addu $t7, $t7, $s0      # $t7 = ADDRESS(I[i, j])

                            # Set I[i, j] = $t6
                                sb $t6, 0($t7)          # I[i, j] = $t6

                            addiu $t2, $t2, 1           # j++
                            j second_for_loop           # re-iterate


                    #if_clause
                    LFSR_CALC:

                            # Allocate space for $t1, $t2 (i, j)
                                sw $t1 20($sp)
                                sw $t2 24($sp)

                            # Call lfsr_random, store result in $v0
                                jal lfsr_random

                            # Reload $t1, $t2 (i, j)
                                lw $t1 20($sp)                  # i = i
                                lw $t2 24($sp)                  # j = j

                            #set I(i,j) to rand && 0xff
                                andi $t3, $v0, 0xFF             # $t3 = lfsr_random && 0xFF

                            #Access I[i, j]
                                mulu $t5, $t2, $s2              # $t5 = j * width
                                addu $t5, $t5, $t1              # $t5 = t5 + i
                                addu $t5, $s0, $t5              # $t5 = ADDRESS(I[i, j])

                            # Set I[i, j]
                                sb $t3, 0($t5)                      # I[i, j] = t3



                    second_for_loop_increment:


                    addiu $t2, $t2, 1                         # j++;
                    j second_for_loop

                    second_for_loop_done:


        addiu $t1, $t1, 1                                    #i++;
        j first_for_loop

        end:

        lw $s0 0($sp)
        lw $s1 4($sp)
        lw $s2 8($sp)
        lw $s3 12($sp)
        lw $s4 16($sp)
        lw $t1 20($sp)
        lw $t2 24($sp)
        lw $ra 28($sp)

        addiu $sp $sp 32
        jr $ra
