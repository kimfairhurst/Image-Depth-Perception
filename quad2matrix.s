.text

# Decodes a quadtree to the original matrix
#
# Arguments:
#     quadtree (qNode*) a0
#     matrix (void*) a1
#     matrix_width (int) a2
#
# Recall that quadtree representation uses the following format:
#     struct qNode {
#         int leaf;
#         int size;
#         int x;
#         int y;
#         int gray_value;
#         qNode *child_NW, *child_NE, *child_SE, *child_SW;
#     }

quad2matrix:

	    beq $a0, 0, the_end

        lw $t1, 0($a0)          # $t1 = leaf
        beq $t1, $0, RECURSION  # if not leaf, go to RECURSION


    BASE_CASE:

        lw $t4, 4($a0)        # t4 = qNode.size
        lw $t6, 8($a0)        # t6 = x
        lw $t1, 12($a0)       # t1 = y
        li $t2, 0             # i = 0;

        first_for_loop:

        beq $t2, $t4, first_for_loop_done       # if i == size, go to first_for_loop_done

                li $t3, 0             # j = 0;
                second_for_loop:

                beq $t3, $t4, second_for_loop_done      # if j = size, go to second_for_loop_done

                #Access Matrix[i, j]
                        addu $t5, $t1, $t2        # t5 = (i + y)
                        mulu $t5, $a2, $t5        # t5 = (i + y) * width
                        add $t5, $t5, $t3         # t5 += j
                        add $t5, $t5, $t6         # t5 += x
                        add $t0, $t5, $a1         # t0 = ADDRESS(matrix[j, i])

                #Assign Matrix Value
                        lb $s1, 16($a0)           # s1 = qNode.gray_value
                        sb $s1, 0($t0)            # matrix[j, i] = t6  

                addiu $t3, $t3, 1                 # j ++;
                j second_for_loop

                second_for_loop_done:

        addiu $t2, $t2, 1                        # i++;
        j first_for_loop

        first_for_loop_done:




    RECURSION:

        # Call quad2matrix on NW
            addi $sp, $sp, -8
            sw $a0, 0($sp) #root pointer
            sw $ra, 4($sp) #ra pointer
            lw $a0 20($a0) #pointer to NW
            jal quad2matrix
            # restore the stack
            lw $a0 0($sp) #reset original root
            lw $ra 4($sp) #reset original ra
            addiu $sp, $sp, 8

        # Call quad2matrix on NE
            addi $sp, $sp, -8
            sw $a0, 0($sp) #root pointer
            sw $ra, 4($sp) #ra pointer
            lw $a0 24($a0) #pointer to NE
            jal quad2matrix
            # restore the stack
            lw $a0 0($sp) #reset original root
            lw $ra 4($sp) #reset original ra
            addiu $sp, $sp, 8

        # Call quad2matrix on SE
            addi $sp, $sp, -8
            sw $a0, 0($sp) #root pointer
            sw $ra, 4($sp) #ra pointer
            lw $a0 28($a0) #pointer to SE
            jal quad2matrix
            # restore the stack
            lw $a0 0($sp) #reset original root
            lw $ra 4($sp) #reset original ra
            addiu $sp, $sp, 8

        # Call quad2matrix on SW
            addi $sp, $sp, -8
            sw $a0, 0($sp) #root pointer
            sw $ra, 4($sp) #ra pointer
            lw $a0 32($a0) #pointer to SW
            jal quad2matrix
            # restore the stack
            lw $a0 0($sp) #reset original root
            lw $ra 4($sp) #reset original ra
            addiu $sp, $sp, 8

the_end:
    jr $ra
