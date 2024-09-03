
flashboot.elf:     file format elf32-littleriscv


Disassembly of section .text.entry:

00a28300 <_start>:
  a28300:	0040006f          	j	a28304 <Reset_Handler>

00a28304 <Reset_Handler>:
  a28304:	00000293          	li	t0,0
  a28308:	3a029073          	csrw	pmpcfg0,t0
  a2830c:	00000293          	li	t0,0
  a28310:	3a129073          	csrw	pmpcfg1,t0
  a28314:	00000293          	li	t0,0
  a28318:	3a229073          	csrw	pmpcfg2,t0
  a2831c:	00000293          	li	t0,0
  a28320:	3a329073          	csrw	pmpcfg3,t0
  a28324:	00000293          	li	t0,0
  a28328:	7d929073          	csrw	0x7d9,t0
  a2832c:	fffdd297          	auipc	t0,0xfffdd
  a28330:	7d828293          	addi	t0,t0,2008 # a05b04 <__bss_end__>
  a28334:	00050313          	mv	t1,a0
  a28338:	00032e03          	lw	t3,0(t1)
  a2833c:	01c2a023          	sw	t3,0(t0)
  a28340:	00009297          	auipc	t0,0x9
  a28344:	19c28293          	addi	t0,t0,412 # a314dc <trap_vector>
  a28348:	00128293          	addi	t0,t0,1
  a2834c:	30529073          	csrw	mtvec,t0
  a28350:	30005073          	csrwi	mstatus,0
  a28354:	30405073          	csrwi	mie,0
  a28358:	0000c197          	auipc	gp,0xc
  a2835c:	46418193          	addi	gp,gp,1124 # a347bc <_gp_>
  a28360:	fffdd117          	auipc	sp,0xfffdd
  a28364:	9a010113          	addi	sp,sp,-1632 # a04d00 <__stack_top__>
  a28368:	fffdb297          	auipc	t0,0xfffdb
  a2836c:	d9828293          	addi	t0,t0,-616 # a03100 <g_system_stack_begin>
  a28370:	fffdd317          	auipc	t1,0xfffdd
  a28374:	19030313          	addi	t1,t1,400 # a05500 <g_flash_cmd_funcs>
  a28378:	00628c63          	beq	t0,t1,a28390 <end_set_stack_loop>
  a2837c:	efbeb3b7          	lui	t2,0xefbeb
  a28380:	dde38393          	addi	t2,t2,-546 # efbeadde <_gp_+0xef1b6622>

00a28384 <set_stack_loop>:
  a28384:	0072a023          	sw	t2,0(t0)
  a28388:	00428293          	addi	t0,t0,4
  a2838c:	fe62cce3          	blt	t0,t1,a28384 <set_stack_loop>

00a28390 <end_set_stack_loop>:
  a28390:	00000093          	li	ra,0
  a28394:	00000213          	li	tp,0
  a28398:	00000413          	li	s0,0
  a2839c:	00000493          	li	s1,0
  a283a0:	00000513          	li	a0,0
  a283a4:	00000593          	li	a1,0
  a283a8:	00000613          	li	a2,0
  a283ac:	00000693          	li	a3,0
  a283b0:	00000713          	li	a4,0
  a283b4:	00000793          	li	a5,0
  a283b8:	00000813          	li	a6,0
  a283bc:	00000893          	li	a7,0
  a283c0:	00000913          	li	s2,0
  a283c4:	00000993          	li	s3,0
  a283c8:	00000a13          	li	s4,0
  a283cc:	00000a93          	li	s5,0
  a283d0:	00000b13          	li	s6,0
  a283d4:	00000b93          	li	s7,0
  a283d8:	00000c13          	li	s8,0
  a283dc:	00000c93          	li	s9,0
  a283e0:	00000d13          	li	s10,0
  a283e4:	00000d93          	li	s11,0
  a283e8:	00000e13          	li	t3,0
  a283ec:	00000e93          	li	t4,0
  a283f0:	00000f13          	li	t5,0
  a283f4:	00000f93          	li	t6,0
  a283f8:	fffdd297          	auipc	t0,0xfffdd
  a283fc:	10828293          	addi	t0,t0,264 # a05500 <g_flash_cmd_funcs>
  a28400:	fffdd317          	auipc	t1,0xfffdd
  a28404:	70430313          	addi	t1,t1,1796 # a05b04 <__bss_end__>
  a28408:	00628a63          	beq	t0,t1,a2841c <end_clear_bss_loop>
  a2840c:	00000393          	li	t2,0

00a28410 <clear_bss_loop>:
  a28410:	0072a023          	sw	t2,0(t0)
  a28414:	00428293          	addi	t0,t0,4
  a28418:	fe62cce3          	blt	t0,t1,a28410 <clear_bss_loop>

00a2841c <end_clear_bss_loop>:
  a2841c:	25d0006f          	j	a28e78 <start_fastboot>

00a28420 <g_ram_exception_table>:
  a28420:	00a313e0 00a2d204 00a2d204 00a2d204     ................
  a28430:	00a2d204 00a2d204 00a2d204 00a2d204     ................
  a28440:	00a2d204 00a2d204 00a2d204 00a2d204     ................
  a28450:	00a2d218 00a2d204 00a2d204 00a2d204     ................
  a28460:	00a2d204 00a2d204 00a2d204 00a2d204     ................
  a28470:	00a2d204 00a2d204 00a2d204 00a2d204     ................
  a28480:	00a2d204 00a2d204 00a2cde0 00a2cde0     ................
  a28490:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a284a0:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a284b0:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a284c0:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a284d0:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a284e0:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a284f0:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a28500:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a28510:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a28520:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a28530:	00a2cde0 00a2cde0 00a2cde0 00a2cde0     ................
  a28540:	00a2cde0 00a2cde0                       ........

Disassembly of section .text:

00a28550 <exc_syscall>:
  a28550:	8082                	ret

00a28552 <handle_syscall>:
  a28552:	0411                	addi	s0,s0,4
  a28554:	c022                	sw	s0,0(sp)
  a28556:	3fed                	jal	ra,a28550 <exc_syscall>
  a28558:	42d2                	lw	t0,20(sp)
  a2855a:	34129073          	csrw	mepc,t0
  a2855e:	5d82                	lw	s11,32(sp)
  a28560:	5d12                	lw	s10,36(sp)
  a28562:	5ca2                	lw	s9,40(sp)
  a28564:	5c32                	lw	s8,44(sp)
  a28566:	5bc2                	lw	s7,48(sp)
  a28568:	5b52                	lw	s6,52(sp)
  a2856a:	5ae2                	lw	s5,56(sp)
  a2856c:	5a72                	lw	s4,60(sp)
  a2856e:	4986                	lw	s3,64(sp)
  a28570:	4916                	lw	s2,68(sp)
  a28572:	44a6                	lw	s1,72(sp)
  a28574:	4436                	lw	s0,76(sp)
  a28576:	4fc6                	lw	t6,80(sp)
  a28578:	4f56                	lw	t5,84(sp)
  a2857a:	4ee6                	lw	t4,88(sp)
  a2857c:	4e76                	lw	t3,92(sp)
  a2857e:	5886                	lw	a7,96(sp)
  a28580:	5816                	lw	a6,100(sp)
  a28582:	57a6                	lw	a5,104(sp)
  a28584:	5736                	lw	a4,108(sp)
  a28586:	56c6                	lw	a3,112(sp)
  a28588:	5656                	lw	a2,116(sp)
  a2858a:	55e6                	lw	a1,120(sp)
  a2858c:	5576                	lw	a0,124(sp)
  a2858e:	438a                	lw	t2,128(sp)
  a28590:	431a                	lw	t1,132(sp)
  a28592:	42aa                	lw	t0,136(sp)
  a28594:	40ba                	lw	ra,140(sp)
  a28596:	6149                	addi	sp,sp,144
  a28598:	30200073          	mret

00a2859c <__udivdi3>:
  a2859c:	4701                	li	a4,0
  a2859e:	a039                	j	a285ac <__udivmoddi4>

00a285a0 <__umoddi3>:
  a285a0:	8118                	push	{ra},-32
  a285a2:	0038                	addi	a4,sp,8
  a285a4:	2021                	jal	ra,a285ac <__udivmoddi4>
  a285a6:	4522                	lw	a0,8(sp)
  a285a8:	45b2                	lw	a1,12(sp)
  a285aa:	8114                	popret	{ra},32

00a285ac <__udivmoddi4>:
  a285ac:	88b6                	mv	a7,a3
  a285ae:	8832                	mv	a6,a2
  a285b0:	87aa                	mv	a5,a0
  a285b2:	8e2e                	mv	t3,a1
  a285b4:	eedd                	bnez	a3,a28672 <__udivmoddi4+0xc6>
  a285b6:	12c5f163          	bgeu	a1,a2,a286d8 <__udivmoddi4+0x12c>
  a285ba:	66c1                	lui	a3,0x10
  a285bc:	1cd66a63          	bltu	a2,a3,a28790 <__udivmoddi4+0x1e4>
  a285c0:	010006b7          	lui	a3,0x1000
  a285c4:	3ad66563          	bltu	a2,a3,a2896e <__udivmoddi4+0x3c2>
  a285c8:	01865313          	srli	t1,a2,0x18
  a285cc:	48e1                	li	a7,24
  a285ce:	0000c697          	auipc	a3,0xc
  a285d2:	9e26a683          	lw	a3,-1566(a3) # a33fb0 <_GLOBAL_OFFSET_TABLE_+0x4>
  a285d6:	969a                	add	a3,a3,t1
  a285d8:	2294                	lbu	a3,0(a3)
  a285da:	02000313          	li	t1,32
  a285de:	98b6                	add	a7,a7,a3
  a285e0:	41130333          	sub	t1,t1,a7
  a285e4:	00030c63          	beqz	t1,a285fc <__udivmoddi4+0x50>
  a285e8:	006595b3          	sll	a1,a1,t1
  a285ec:	011558b3          	srl	a7,a0,a7
  a285f0:	00661833          	sll	a6,a2,t1
  a285f4:	00b8ee33          	or	t3,a7,a1
  a285f8:	006517b3          	sll	a5,a0,t1
  a285fc:	01085e93          	srli	t4,a6,0x10
  a28600:	03de5f33          	divu	t5,t3,t4
  a28604:	01081613          	slli	a2,a6,0x10
  a28608:	8241                	srli	a2,a2,0x10
  a2860a:	0107d693          	srli	a3,a5,0x10
  a2860e:	03de75b3          	remu	a1,t3,t4
  a28612:	03e60fb3          	mul	t6,a2,t5
  a28616:	20b6a69b          	orshf	a3,a3,a1,sll,16
  a2861a:	01f6fa63          	bgeu	a3,t6,a2862e <__udivmoddi4+0x82>
  a2861e:	96c2                	add	a3,a3,a6
  a28620:	ffff0593          	addi	a1,t5,-1
  a28624:	0106e463          	bltu	a3,a6,a2862c <__udivmoddi4+0x80>
  a28628:	37f6e563          	bltu	a3,t6,a28992 <__udivmoddi4+0x3e6>
  a2862c:	8f2e                	mv	t5,a1
  a2862e:	41f685b3          	sub	a1,a3,t6
  a28632:	03d5d533          	divu	a0,a1,t4
  a28636:	01079693          	slli	a3,a5,0x10
  a2863a:	82c1                	srli	a3,a3,0x10
  a2863c:	03d5f5b3          	remu	a1,a1,t4
  a28640:	02a60633          	mul	a2,a2,a0
  a28644:	20b6a79b          	orshf	a5,a3,a1,sll,16
  a28648:	00c7fa63          	bgeu	a5,a2,a2865c <__udivmoddi4+0xb0>
  a2864c:	97c2                	add	a5,a5,a6
  a2864e:	fff50693          	addi	a3,a0,-1
  a28652:	0107e463          	bltu	a5,a6,a2865a <__udivmoddi4+0xae>
  a28656:	32c7eb63          	bltu	a5,a2,a2898c <__udivmoddi4+0x3e0>
  a2865a:	8536                	mv	a0,a3
  a2865c:	8f91                	sub	a5,a5,a2
  a2865e:	21e5251b          	orshf	a0,a0,t5,sll,16
  a28662:	4581                	li	a1,0
  a28664:	c711                	beqz	a4,a28670 <__udivmoddi4+0xc4>
  a28666:	0067d7b3          	srl	a5,a5,t1
  a2866a:	c31c                	sw	a5,0(a4)
  a2866c:	00072223          	sw	zero,4(a4)
  a28670:	8082                	ret
  a28672:	00d5f963          	bgeu	a1,a3,a28684 <__udivmoddi4+0xd8>
  a28676:	12070f63          	beqz	a4,a287b4 <__udivmoddi4+0x208>
  a2867a:	c308                	sw	a0,0(a4)
  a2867c:	c34c                	sw	a1,4(a4)
  a2867e:	4501                	li	a0,0
  a28680:	4581                	li	a1,0
  a28682:	8082                	ret
  a28684:	67c1                	lui	a5,0x10
  a28686:	0ef6ed63          	bltu	a3,a5,a28780 <__udivmoddi4+0x1d4>
  a2868a:	010007b7          	lui	a5,0x1000
  a2868e:	2cf6ea63          	bltu	a3,a5,a28962 <__udivmoddi4+0x3b6>
  a28692:	0186d893          	srli	a7,a3,0x18
  a28696:	4861                	li	a6,24
  a28698:	0000c797          	auipc	a5,0xc
  a2869c:	9187a783          	lw	a5,-1768(a5) # a33fb0 <_GLOBAL_OFFSET_TABLE_+0x4>
  a286a0:	97c6                	add	a5,a5,a7
  a286a2:	0007c303          	lbu	t1,0(a5)
  a286a6:	02000e93          	li	t4,32
  a286aa:	9342                	add	t1,t1,a6
  a286ac:	406e8eb3          	sub	t4,t4,t1
  a286b0:	180e9563          	bnez	t4,a2883a <__udivmoddi4+0x28e>
  a286b4:	00b6e463          	bltu	a3,a1,a286bc <__udivmoddi4+0x110>
  a286b8:	2ec56363          	bltu	a0,a2,a2899e <__udivmoddi4+0x3f2>
  a286bc:	40c50633          	sub	a2,a0,a2
  a286c0:	00c53533          	sltu	a0,a0,a2
  a286c4:	8d95                	sub	a1,a1,a3
  a286c6:	40a58e33          	sub	t3,a1,a0
  a286ca:	4505                	li	a0,1
  a286cc:	4581                	li	a1,0
  a286ce:	d34d                	beqz	a4,a28670 <__udivmoddi4+0xc4>
  a286d0:	c310                	sw	a2,0(a4)
  a286d2:	01c72223          	sw	t3,4(a4)
  a286d6:	8082                	ret
  a286d8:	e601                	bnez	a2,a286e0 <__udivmoddi4+0x134>
  a286da:	4685                	li	a3,1
  a286dc:	02c6d833          	divu	a6,a3,a2
  a286e0:	66c1                	lui	a3,0x10
  a286e2:	0cd86063          	bltu	a6,a3,a287a2 <__udivmoddi4+0x1f6>
  a286e6:	010006b7          	lui	a3,0x1000
  a286ea:	26d86863          	bltu	a6,a3,a2895a <__udivmoddi4+0x3ae>
  a286ee:	01885613          	srli	a2,a6,0x18
  a286f2:	48e1                	li	a7,24
  a286f4:	0000c697          	auipc	a3,0xc
  a286f8:	8bc6a683          	lw	a3,-1860(a3) # a33fb0 <_GLOBAL_OFFSET_TABLE_+0x4>
  a286fc:	96b2                	add	a3,a3,a2
  a286fe:	2290                	lbu	a2,0(a3)
  a28700:	02000313          	li	t1,32
  a28704:	9646                	add	a2,a2,a7
  a28706:	40c30333          	sub	t1,t1,a2
  a2870a:	0a031863          	bnez	t1,a287ba <__udivmoddi4+0x20e>
  a2870e:	01081e93          	slli	t4,a6,0x10
  a28712:	410586b3          	sub	a3,a1,a6
  a28716:	01085f13          	srli	t5,a6,0x10
  a2871a:	010ede93          	srli	t4,t4,0x10
  a2871e:	4585                	li	a1,1
  a28720:	0107d893          	srli	a7,a5,0x10
  a28724:	03e6de33          	divu	t3,a3,t5
  a28728:	03e6f6b3          	remu	a3,a3,t5
  a2872c:	03de0633          	mul	a2,t3,t4
  a28730:	20d8a69b          	orshf	a3,a7,a3,sll,16
  a28734:	00c6fa63          	bgeu	a3,a2,a28748 <__udivmoddi4+0x19c>
  a28738:	96c2                	add	a3,a3,a6
  a2873a:	fffe0513          	addi	a0,t3,-1
  a2873e:	0106e463          	bltu	a3,a6,a28746 <__udivmoddi4+0x19a>
  a28742:	24c6eb63          	bltu	a3,a2,a28998 <__udivmoddi4+0x3ec>
  a28746:	8e2a                	mv	t3,a0
  a28748:	40c68633          	sub	a2,a3,a2
  a2874c:	03e65533          	divu	a0,a2,t5
  a28750:	01079693          	slli	a3,a5,0x10
  a28754:	82c1                	srli	a3,a3,0x10
  a28756:	03e67633          	remu	a2,a2,t5
  a2875a:	03d508b3          	mul	a7,a0,t4
  a2875e:	20c6a79b          	orshf	a5,a3,a2,sll,16
  a28762:	0117fa63          	bgeu	a5,a7,a28776 <__udivmoddi4+0x1ca>
  a28766:	97c2                	add	a5,a5,a6
  a28768:	fff50693          	addi	a3,a0,-1
  a2876c:	0107e463          	bltu	a5,a6,a28774 <__udivmoddi4+0x1c8>
  a28770:	2117eb63          	bltu	a5,a7,a28986 <__udivmoddi4+0x3da>
  a28774:	8536                	mv	a0,a3
  a28776:	411787b3          	sub	a5,a5,a7
  a2877a:	21c5251b          	orshf	a0,a0,t3,sll,16
  a2877e:	b5dd                	j	a28664 <__udivmoddi4+0xb8>
  a28780:	0ff00793          	li	a5,255
  a28784:	1ed7f363          	bgeu	a5,a3,a2896a <__udivmoddi4+0x3be>
  a28788:	0086d893          	srli	a7,a3,0x8
  a2878c:	4821                	li	a6,8
  a2878e:	b729                	j	a28698 <__udivmoddi4+0xec>
  a28790:	8332                	mv	t1,a2
  a28792:	0ff00693          	li	a3,255
  a28796:	e2c6fce3          	bgeu	a3,a2,a285ce <__udivmoddi4+0x22>
  a2879a:	00865313          	srli	t1,a2,0x8
  a2879e:	48a1                	li	a7,8
  a287a0:	b53d                	j	a285ce <__udivmoddi4+0x22>
  a287a2:	8642                	mv	a2,a6
  a287a4:	0ff00693          	li	a3,255
  a287a8:	f506f6e3          	bgeu	a3,a6,a286f4 <__udivmoddi4+0x148>
  a287ac:	00885613          	srli	a2,a6,0x8
  a287b0:	48a1                	li	a7,8
  a287b2:	b789                	j	a286f4 <__udivmoddi4+0x148>
  a287b4:	4581                	li	a1,0
  a287b6:	4501                	li	a0,0
  a287b8:	8082                	ret
  a287ba:	00681833          	sll	a6,a6,t1
  a287be:	00c5d8b3          	srl	a7,a1,a2
  a287c2:	01085f13          	srli	t5,a6,0x10
  a287c6:	03e8dfb3          	divu	t6,a7,t5
  a287ca:	01081e93          	slli	t4,a6,0x10
  a287ce:	006595b3          	sll	a1,a1,t1
  a287d2:	00c55633          	srl	a2,a0,a2
  a287d6:	010ede93          	srli	t4,t4,0x10
  a287da:	8e4d                	or	a2,a2,a1
  a287dc:	01065e13          	srli	t3,a2,0x10
  a287e0:	006517b3          	sll	a5,a0,t1
  a287e4:	03e8f8b3          	remu	a7,a7,t5
  a287e8:	03fe85b3          	mul	a1,t4,t6
  a287ec:	211e251b          	orshf	a0,t3,a7,sll,16
  a287f0:	00b57b63          	bgeu	a0,a1,a28806 <__udivmoddi4+0x25a>
  a287f4:	9542                	add	a0,a0,a6
  a287f6:	ffff8693          	addi	a3,t6,-1
  a287fa:	19056463          	bltu	a0,a6,a28982 <__udivmoddi4+0x3d6>
  a287fe:	18b57263          	bgeu	a0,a1,a28982 <__udivmoddi4+0x3d6>
  a28802:	1ff9                	addi	t6,t6,-2
  a28804:	9542                	add	a0,a0,a6
  a28806:	8d0d                	sub	a0,a0,a1
  a28808:	03e558b3          	divu	a7,a0,t5
  a2880c:	9e21                	uxth	a2
  a2880e:	03e57533          	remu	a0,a0,t5
  a28812:	031e86b3          	mul	a3,t4,a7
  a28816:	20a6259b          	orshf	a1,a2,a0,sll,16
  a2881a:	00d5fb63          	bgeu	a1,a3,a28830 <__udivmoddi4+0x284>
  a2881e:	95c2                	add	a1,a1,a6
  a28820:	fff88613          	addi	a2,a7,-1
  a28824:	1505eb63          	bltu	a1,a6,a2897a <__udivmoddi4+0x3ce>
  a28828:	14d5f963          	bgeu	a1,a3,a2897a <__udivmoddi4+0x3ce>
  a2882c:	18f9                	addi	a7,a7,-2
  a2882e:	95c2                	add	a1,a1,a6
  a28830:	40d586b3          	sub	a3,a1,a3
  a28834:	21f8a59b          	orshf	a1,a7,t6,sll,16
  a28838:	b5e5                	j	a28720 <__udivmoddi4+0x174>
  a2883a:	00665833          	srl	a6,a2,t1
  a2883e:	01d696b3          	sll	a3,a3,t4
  a28842:	00d866b3          	or	a3,a6,a3
  a28846:	0065d7b3          	srl	a5,a1,t1
  a2884a:	0106de13          	srli	t3,a3,0x10
  a2884e:	03c7dfb3          	divu	t6,a5,t3
  a28852:	01069893          	slli	a7,a3,0x10
  a28856:	0108d893          	srli	a7,a7,0x10
  a2885a:	00655833          	srl	a6,a0,t1
  a2885e:	01d51f33          	sll	t5,a0,t4
  a28862:	01d595b3          	sll	a1,a1,t4
  a28866:	00b865b3          	or	a1,a6,a1
  a2886a:	0105d813          	srli	a6,a1,0x10
  a2886e:	01d61633          	sll	a2,a2,t4
  a28872:	03c7f7b3          	remu	a5,a5,t3
  a28876:	03f88533          	mul	a0,a7,t6
  a2887a:	20f8279b          	orshf	a5,a6,a5,sll,16
  a2887e:	00a7fb63          	bgeu	a5,a0,a28894 <__udivmoddi4+0x2e8>
  a28882:	97b6                	add	a5,a5,a3
  a28884:	ffff8813          	addi	a6,t6,-1
  a28888:	0ed7eb63          	bltu	a5,a3,a2897e <__udivmoddi4+0x3d2>
  a2888c:	0ea7f963          	bgeu	a5,a0,a2897e <__udivmoddi4+0x3d2>
  a28890:	1ff9                	addi	t6,t6,-2
  a28892:	97b6                	add	a5,a5,a3
  a28894:	8f89                	sub	a5,a5,a0
  a28896:	03c7d533          	divu	a0,a5,t3
  a2889a:	9da1                	uxth	a1
  a2889c:	03c7f7b3          	remu	a5,a5,t3
  a288a0:	02a888b3          	mul	a7,a7,a0
  a288a4:	20f5a59b          	orshf	a1,a1,a5,sll,16
  a288a8:	0115fb63          	bgeu	a1,a7,a288be <__udivmoddi4+0x312>
  a288ac:	95b6                	add	a1,a1,a3
  a288ae:	fff50793          	addi	a5,a0,-1
  a288b2:	0cd5e263          	bltu	a1,a3,a28976 <__udivmoddi4+0x3ca>
  a288b6:	0d15f063          	bgeu	a1,a7,a28976 <__udivmoddi4+0x3ca>
  a288ba:	1579                	addi	a0,a0,-2
  a288bc:	95b6                	add	a1,a1,a3
  a288be:	63c1                	lui	t2,0x10
  a288c0:	21f5251b          	orshf	a0,a0,t6,sll,16
  a288c4:	fff38793          	addi	a5,t2,-1 # ffff <g_intheap_size+0x3>
  a288c8:	00f57833          	and	a6,a0,a5
  a288cc:	01055e13          	srli	t3,a0,0x10
  a288d0:	8ff1                	and	a5,a5,a2
  a288d2:	01065f93          	srli	t6,a2,0x10
  a288d6:	02fe02b3          	mul	t0,t3,a5
  a288da:	411588b3          	sub	a7,a1,a7
  a288de:	03f805b3          	mul	a1,a6,t6
  a288e2:	02f80833          	mul	a6,a6,a5
  a288e6:	9596                	add	a1,a1,t0
  a288e8:	6105859b          	addshf	a1,a1,a6,srl,16
  a288ec:	03fe0e33          	mul	t3,t3,t6
  a288f0:	0055f363          	bgeu	a1,t0,a288f6 <__udivmoddi4+0x34a>
  a288f4:	9e1e                	add	t3,t3,t2
  a288f6:	67c1                	lui	a5,0x10
  a288f8:	17fd                	addi	a5,a5,-1 # ffff <g_intheap_size+0x3>
  a288fa:	00f5ffb3          	and	t6,a1,a5
  a288fe:	00f877b3          	and	a5,a6,a5
  a28902:	60be081b          	addshf	a6,t3,a1,srl,16
  a28906:	21f7879b          	addshf	a5,a5,t6,sll,16
  a2890a:	0308ed63          	bltu	a7,a6,a28944 <__udivmoddi4+0x398>
  a2890e:	03088963          	beq	a7,a6,a28940 <__udivmoddi4+0x394>
  a28912:	4581                	li	a1,0
  a28914:	d4070ee3          	beqz	a4,a28670 <__udivmoddi4+0xc4>
  a28918:	40ff07b3          	sub	a5,t5,a5
  a2891c:	00ff3f33          	sltu	t5,t5,a5
  a28920:	410888b3          	sub	a7,a7,a6
  a28924:	41e888b3          	sub	a7,a7,t5
  a28928:	00689333          	sll	t1,a7,t1
  a2892c:	01d7d7b3          	srl	a5,a5,t4
  a28930:	00f367b3          	or	a5,t1,a5
  a28934:	01d8d8b3          	srl	a7,a7,t4
  a28938:	c31c                	sw	a5,0(a4)
  a2893a:	01172223          	sw	a7,4(a4)
  a2893e:	8082                	ret
  a28940:	fcff79e3          	bgeu	t5,a5,a28912 <__udivmoddi4+0x366>
  a28944:	40c78633          	sub	a2,a5,a2
  a28948:	00c7b7b3          	sltu	a5,a5,a2
  a2894c:	40d80833          	sub	a6,a6,a3
  a28950:	40f80833          	sub	a6,a6,a5
  a28954:	157d                	addi	a0,a0,-1
  a28956:	87b2                	mv	a5,a2
  a28958:	bf6d                	j	a28912 <__udivmoddi4+0x366>
  a2895a:	01085613          	srli	a2,a6,0x10
  a2895e:	48c1                	li	a7,16
  a28960:	bb51                	j	a286f4 <__udivmoddi4+0x148>
  a28962:	0106d893          	srli	a7,a3,0x10
  a28966:	4841                	li	a6,16
  a28968:	bb05                	j	a28698 <__udivmoddi4+0xec>
  a2896a:	4801                	li	a6,0
  a2896c:	b335                	j	a28698 <__udivmoddi4+0xec>
  a2896e:	01065313          	srli	t1,a2,0x10
  a28972:	48c1                	li	a7,16
  a28974:	b9a9                	j	a285ce <__udivmoddi4+0x22>
  a28976:	853e                	mv	a0,a5
  a28978:	b799                	j	a288be <__udivmoddi4+0x312>
  a2897a:	88b2                	mv	a7,a2
  a2897c:	bd55                	j	a28830 <__udivmoddi4+0x284>
  a2897e:	8fc2                	mv	t6,a6
  a28980:	bf11                	j	a28894 <__udivmoddi4+0x2e8>
  a28982:	8fb6                	mv	t6,a3
  a28984:	b549                	j	a28806 <__udivmoddi4+0x25a>
  a28986:	1579                	addi	a0,a0,-2
  a28988:	97c2                	add	a5,a5,a6
  a2898a:	b3f5                	j	a28776 <__udivmoddi4+0x1ca>
  a2898c:	1579                	addi	a0,a0,-2
  a2898e:	97c2                	add	a5,a5,a6
  a28990:	b1f1                	j	a2865c <__udivmoddi4+0xb0>
  a28992:	1f79                	addi	t5,t5,-2
  a28994:	96c2                	add	a3,a3,a6
  a28996:	b961                	j	a2862e <__udivmoddi4+0x82>
  a28998:	1e79                	addi	t3,t3,-2
  a2899a:	96c2                	add	a3,a3,a6
  a2899c:	b375                	j	a28748 <__udivmoddi4+0x19c>
  a2899e:	862a                	mv	a2,a0
  a289a0:	4501                	li	a0,0
  a289a2:	b32d                	j	a286cc <__udivmoddi4+0x120>

00a289a4 <memset>:
  a289a4:	ca59                	beqz	a2,a28a3a <memset+0x96>
  a289a6:	0ff5f793          	andi	a5,a1,255
  a289aa:	a11c                	sb	a5,0(a0)
  a289ac:	00c50733          	add	a4,a0,a2
  a289b0:	fef70fa3          	sb	a5,-1(a4)
  a289b4:	032661bb          	bltui	a2,3,a28a3a <memset+0x96>
  a289b8:	b11c                	sb	a5,1(a0)
  a289ba:	a13c                	sb	a5,2(a0)
  a289bc:	fef70f23          	sb	a5,-2(a4)
  a289c0:	fef70ea3          	sb	a5,-3(a4)
  a289c4:	07166dbb          	bltui	a2,7,a28a3a <memset+0x96>
  a289c8:	b13c                	sb	a5,3(a0)
  a289ca:	fef70e23          	sb	a5,-4(a4)
  a289ce:	46a1                	li	a3,8
  a289d0:	06c6f563          	bgeu	a3,a2,a28a3a <memset+0x96>
  a289d4:	40a007b3          	neg	a5,a0
  a289d8:	8b8d                	andi	a5,a5,3
  a289da:	00f50733          	add	a4,a0,a5
  a289de:	8e1d                	sub	a2,a2,a5
  a289e0:	0ff5f593          	andi	a1,a1,255
  a289e4:	0101 0101 079f      	l.li	a5,0x1010101
  a289ea:	02f585b3          	mul	a1,a1,a5
  a289ee:	9a71                	andi	a2,a2,-4
  a289f0:	00c707b3          	add	a5,a4,a2
  a289f4:	c30c                	sw	a1,0(a4)
  a289f6:	feb7ae23          	sw	a1,-4(a5)
  a289fa:	04c6f063          	bgeu	a3,a2,a28a3a <memset+0x96>
  a289fe:	c34c                	sw	a1,4(a4)
  a28a00:	c70c                	sw	a1,8(a4)
  a28a02:	feb7aa23          	sw	a1,-12(a5)
  a28a06:	feb7ac23          	sw	a1,-8(a5)
  a28a0a:	19066c3b          	bltui	a2,25,a28a3a <memset+0x96>
  a28a0e:	c74c                	sw	a1,12(a4)
  a28a10:	cb0c                	sw	a1,16(a4)
  a28a12:	cb4c                	sw	a1,20(a4)
  a28a14:	cf0c                	sw	a1,24(a4)
  a28a16:	feb7a223          	sw	a1,-28(a5)
  a28a1a:	feb7a423          	sw	a1,-24(a5)
  a28a1e:	feb7a623          	sw	a1,-20(a5)
  a28a22:	feb7a823          	sw	a1,-16(a5)
  a28a26:	00477793          	andi	a5,a4,4
  a28a2a:	07e1                	addi	a5,a5,24
  a28a2c:	97ba                	add	a5,a5,a4
  a28a2e:	46fd                	li	a3,31
  a28a30:	9732                	add	a4,a4,a2
  a28a32:	40f70633          	sub	a2,a4,a5
  a28a36:	00c6e363          	bltu	a3,a2,a28a3c <memset+0x98>
  a28a3a:	8082                	ret
  a28a3c:	c38c                	sw	a1,0(a5)
  a28a3e:	c3cc                	sw	a1,4(a5)
  a28a40:	c78c                	sw	a1,8(a5)
  a28a42:	c7cc                	sw	a1,12(a5)
  a28a44:	cb8c                	sw	a1,16(a5)
  a28a46:	cbcc                	sw	a1,20(a5)
  a28a48:	cf8c                	sw	a1,24(a5)
  a28a4a:	cfcc                	sw	a1,28(a5)
  a28a4c:	02078793          	addi	a5,a5,32
  a28a50:	b7cd                	j	a28a32 <memset+0x8e>

00a28a52 <memcpy>:
  a28a52:	87aa                	mv	a5,a0
  a28a54:	0035f713          	andi	a4,a1,3
  a28a58:	c319                	beqz	a4,a28a5e <memcpy+0xc>
  a28a5a:	16061963          	bnez	a2,a28bcc <memcpy+0x17a>
  a28a5e:	0037f813          	andi	a6,a5,3
  a28a62:	473d                	li	a4,15
  a28a64:	18080e63          	beqz	a6,a28c00 <memcpy+0x1ae>
  a28a68:	20166abb          	bltui	a2,32,a28ad2 <memcpy+0x80>
  a28a6c:	2194                	lbu	a3,0(a1)
  a28a6e:	4198                	lw	a4,0(a1)
  a28a70:	027807bb          	beqi	a6,2,a28c4e <memcpy+0x1fc>
  a28a74:	488d                	li	a7,3
  a28a76:	23180563          	beq	a6,a7,a28ca0 <memcpy+0x24e>
  a28a7a:	a394                	sb	a3,0(a5)
  a28a7c:	3194                	lbu	a3,1(a1)
  a28a7e:	1675                	addi	a2,a2,-3
  a28a80:	058d                	addi	a1,a1,3
  a28a82:	b394                	sb	a3,1(a5)
  a28a84:	fff5c683          	lbu	a3,-1(a1)
  a28a88:	078d                	addi	a5,a5,3
  a28a8a:	4e41                	li	t3,16
  a28a8c:	fed78fa3          	sb	a3,-1(a5)
  a28a90:	0015a303          	lw	t1,1(a1)
  a28a94:	0055a883          	lw	a7,5(a1)
  a28a98:	1641                	addi	a2,a2,-16
  a28a9a:	00831813          	slli	a6,t1,0x8
  a28a9e:	70e8271b          	orshf	a4,a6,a4,srl,24
  a28aa2:	0095a803          	lw	a6,9(a1)
  a28aa6:	c398                	sw	a4,0(a5)
  a28aa8:	00889693          	slli	a3,a7,0x8
  a28aac:	00881713          	slli	a4,a6,0x8
  a28ab0:	7117271b          	orshf	a4,a4,a7,srl,24
  a28ab4:	c798                	sw	a4,8(a5)
  a28ab6:	00d5a703          	lw	a4,13(a1)
  a28aba:	7066a69b          	orshf	a3,a3,t1,srl,24
  a28abe:	c3d4                	sw	a3,4(a5)
  a28ac0:	00871693          	slli	a3,a4,0x8
  a28ac4:	7106a69b          	orshf	a3,a3,a6,srl,24
  a28ac8:	c7d4                	sw	a3,12(a5)
  a28aca:	05c1                	addi	a1,a1,16
  a28acc:	07c1                	addi	a5,a5,16
  a28ace:	fcce61e3          	bltu	t3,a2,a28a90 <memcpy+0x3e>
  a28ad2:	01067713          	andi	a4,a2,16
  a28ad6:	c351                	beqz	a4,a28b5a <memcpy+0x108>
  a28ad8:	2198                	lbu	a4,0(a1)
  a28ada:	07c1                	addi	a5,a5,16
  a28adc:	05c1                	addi	a1,a1,16
  a28ade:	fee78823          	sb	a4,-16(a5)
  a28ae2:	ff15c703          	lbu	a4,-15(a1)
  a28ae6:	fee788a3          	sb	a4,-15(a5)
  a28aea:	ff25c703          	lbu	a4,-14(a1)
  a28aee:	fee78923          	sb	a4,-14(a5)
  a28af2:	ff35c703          	lbu	a4,-13(a1)
  a28af6:	fee789a3          	sb	a4,-13(a5)
  a28afa:	ff45c703          	lbu	a4,-12(a1)
  a28afe:	fee78a23          	sb	a4,-12(a5)
  a28b02:	ff55c703          	lbu	a4,-11(a1)
  a28b06:	fee78aa3          	sb	a4,-11(a5)
  a28b0a:	ff65c703          	lbu	a4,-10(a1)
  a28b0e:	fee78b23          	sb	a4,-10(a5)
  a28b12:	ff75c703          	lbu	a4,-9(a1)
  a28b16:	fee78ba3          	sb	a4,-9(a5)
  a28b1a:	ff85c703          	lbu	a4,-8(a1)
  a28b1e:	fee78c23          	sb	a4,-8(a5)
  a28b22:	ff95c703          	lbu	a4,-7(a1)
  a28b26:	fee78ca3          	sb	a4,-7(a5)
  a28b2a:	ffa5c703          	lbu	a4,-6(a1)
  a28b2e:	fee78d23          	sb	a4,-6(a5)
  a28b32:	ffb5c703          	lbu	a4,-5(a1)
  a28b36:	fee78da3          	sb	a4,-5(a5)
  a28b3a:	ffc5c703          	lbu	a4,-4(a1)
  a28b3e:	fee78e23          	sb	a4,-4(a5)
  a28b42:	ffd5c703          	lbu	a4,-3(a1)
  a28b46:	fee78ea3          	sb	a4,-3(a5)
  a28b4a:	ffe5c703          	lbu	a4,-2(a1)
  a28b4e:	fee78f23          	sb	a4,-2(a5)
  a28b52:	fff5c703          	lbu	a4,-1(a1)
  a28b56:	fee78fa3          	sb	a4,-1(a5)
  a28b5a:	00867713          	andi	a4,a2,8
  a28b5e:	c331                	beqz	a4,a28ba2 <memcpy+0x150>
  a28b60:	2198                	lbu	a4,0(a1)
  a28b62:	07a1                	addi	a5,a5,8
  a28b64:	05a1                	addi	a1,a1,8
  a28b66:	fee78c23          	sb	a4,-8(a5)
  a28b6a:	ff95c703          	lbu	a4,-7(a1)
  a28b6e:	fee78ca3          	sb	a4,-7(a5)
  a28b72:	ffa5c703          	lbu	a4,-6(a1)
  a28b76:	fee78d23          	sb	a4,-6(a5)
  a28b7a:	ffb5c703          	lbu	a4,-5(a1)
  a28b7e:	fee78da3          	sb	a4,-5(a5)
  a28b82:	ffc5c703          	lbu	a4,-4(a1)
  a28b86:	fee78e23          	sb	a4,-4(a5)
  a28b8a:	ffd5c703          	lbu	a4,-3(a1)
  a28b8e:	fee78ea3          	sb	a4,-3(a5)
  a28b92:	ffe5c703          	lbu	a4,-2(a1)
  a28b96:	fee78f23          	sb	a4,-2(a5)
  a28b9a:	fff5c703          	lbu	a4,-1(a1)
  a28b9e:	fee78fa3          	sb	a4,-1(a5)
  a28ba2:	00467713          	andi	a4,a2,4
  a28ba6:	c359                	beqz	a4,a28c2c <memcpy+0x1da>
  a28ba8:	2198                	lbu	a4,0(a1)
  a28baa:	0791                	addi	a5,a5,4
  a28bac:	0591                	addi	a1,a1,4
  a28bae:	fee78e23          	sb	a4,-4(a5)
  a28bb2:	ffd5c703          	lbu	a4,-3(a1)
  a28bb6:	fee78ea3          	sb	a4,-3(a5)
  a28bba:	ffe5c703          	lbu	a4,-2(a1)
  a28bbe:	fee78f23          	sb	a4,-2(a5)
  a28bc2:	fff5c703          	lbu	a4,-1(a1)
  a28bc6:	fee78fa3          	sb	a4,-1(a5)
  a28bca:	a08d                	j	a28c2c <memcpy+0x1da>
  a28bcc:	0585                	addi	a1,a1,1
  a28bce:	fff5c703          	lbu	a4,-1(a1)
  a28bd2:	0785                	addi	a5,a5,1
  a28bd4:	167d                	addi	a2,a2,-1
  a28bd6:	fee78fa3          	sb	a4,-1(a5)
  a28bda:	bdad                	j	a28a54 <memcpy+0x2>
  a28bdc:	4194                	lw	a3,0(a1)
  a28bde:	07c1                	addi	a5,a5,16
  a28be0:	05c1                	addi	a1,a1,16
  a28be2:	fed7a823          	sw	a3,-16(a5)
  a28be6:	ff45a683          	lw	a3,-12(a1)
  a28bea:	1641                	addi	a2,a2,-16
  a28bec:	fed7aa23          	sw	a3,-12(a5)
  a28bf0:	ff85a683          	lw	a3,-8(a1)
  a28bf4:	fed7ac23          	sw	a3,-8(a5)
  a28bf8:	ffc5a683          	lw	a3,-4(a1)
  a28bfc:	fed7ae23          	sw	a3,-4(a5)
  a28c00:	fcc76ee3          	bltu	a4,a2,a28bdc <memcpy+0x18a>
  a28c04:	00867713          	andi	a4,a2,8
  a28c08:	cb11                	beqz	a4,a28c1c <memcpy+0x1ca>
  a28c0a:	4198                	lw	a4,0(a1)
  a28c0c:	07a1                	addi	a5,a5,8
  a28c0e:	05a1                	addi	a1,a1,8
  a28c10:	fee7ac23          	sw	a4,-8(a5)
  a28c14:	ffc5a703          	lw	a4,-4(a1)
  a28c18:	fee7ae23          	sw	a4,-4(a5)
  a28c1c:	00467713          	andi	a4,a2,4
  a28c20:	c711                	beqz	a4,a28c2c <memcpy+0x1da>
  a28c22:	4198                	lw	a4,0(a1)
  a28c24:	0791                	addi	a5,a5,4
  a28c26:	0591                	addi	a1,a1,4
  a28c28:	fee7ae23          	sw	a4,-4(a5)
  a28c2c:	00267713          	andi	a4,a2,2
  a28c30:	cb11                	beqz	a4,a28c44 <memcpy+0x1f2>
  a28c32:	2198                	lbu	a4,0(a1)
  a28c34:	0789                	addi	a5,a5,2
  a28c36:	0589                	addi	a1,a1,2
  a28c38:	fee78f23          	sb	a4,-2(a5)
  a28c3c:	fff5c703          	lbu	a4,-1(a1)
  a28c40:	fee78fa3          	sb	a4,-1(a5)
  a28c44:	8a05                	andi	a2,a2,1
  a28c46:	c219                	beqz	a2,a28c4c <memcpy+0x1fa>
  a28c48:	2198                	lbu	a4,0(a1)
  a28c4a:	a398                	sb	a4,0(a5)
  a28c4c:	8082                	ret
  a28c4e:	a394                	sb	a3,0(a5)
  a28c50:	3194                	lbu	a3,1(a1)
  a28c52:	1679                	addi	a2,a2,-2
  a28c54:	0589                	addi	a1,a1,2
  a28c56:	b394                	sb	a3,1(a5)
  a28c58:	4e45                	li	t3,17
  a28c5a:	0789                	addi	a5,a5,2
  a28c5c:	0025a303          	lw	t1,2(a1)
  a28c60:	0065a883          	lw	a7,6(a1)
  a28c64:	1641                	addi	a2,a2,-16
  a28c66:	01031813          	slli	a6,t1,0x10
  a28c6a:	60e8271b          	orshf	a4,a6,a4,srl,16
  a28c6e:	00a5a803          	lw	a6,10(a1)
  a28c72:	c398                	sw	a4,0(a5)
  a28c74:	01089693          	slli	a3,a7,0x10
  a28c78:	01081713          	slli	a4,a6,0x10
  a28c7c:	6117271b          	orshf	a4,a4,a7,srl,16
  a28c80:	c798                	sw	a4,8(a5)
  a28c82:	00e5a703          	lw	a4,14(a1)
  a28c86:	6066a69b          	orshf	a3,a3,t1,srl,16
  a28c8a:	c3d4                	sw	a3,4(a5)
  a28c8c:	01071693          	slli	a3,a4,0x10
  a28c90:	6106a69b          	orshf	a3,a3,a6,srl,16
  a28c94:	c7d4                	sw	a3,12(a5)
  a28c96:	05c1                	addi	a1,a1,16
  a28c98:	07c1                	addi	a5,a5,16
  a28c9a:	fcce61e3          	bltu	t3,a2,a28c5c <memcpy+0x20a>
  a28c9e:	bd15                	j	a28ad2 <memcpy+0x80>
  a28ca0:	a394                	sb	a3,0(a5)
  a28ca2:	0585                	addi	a1,a1,1
  a28ca4:	167d                	addi	a2,a2,-1
  a28ca6:	0785                	addi	a5,a5,1
  a28ca8:	4e49                	li	t3,18
  a28caa:	0035a303          	lw	t1,3(a1)
  a28cae:	0075a883          	lw	a7,7(a1)
  a28cb2:	1641                	addi	a2,a2,-16
  a28cb4:	01831813          	slli	a6,t1,0x18
  a28cb8:	50e8271b          	orshf	a4,a6,a4,srl,8
  a28cbc:	00b5a803          	lw	a6,11(a1)
  a28cc0:	c398                	sw	a4,0(a5)
  a28cc2:	01889693          	slli	a3,a7,0x18
  a28cc6:	01881713          	slli	a4,a6,0x18
  a28cca:	5117271b          	orshf	a4,a4,a7,srl,8
  a28cce:	c798                	sw	a4,8(a5)
  a28cd0:	00f5a703          	lw	a4,15(a1)
  a28cd4:	5066a69b          	orshf	a3,a3,t1,srl,8
  a28cd8:	c3d4                	sw	a3,4(a5)
  a28cda:	01871693          	slli	a3,a4,0x18
  a28cde:	5106a69b          	orshf	a3,a3,a6,srl,8
  a28ce2:	c7d4                	sw	a3,12(a5)
  a28ce4:	05c1                	addi	a1,a1,16
  a28ce6:	07c1                	addi	a5,a5,16
  a28ce8:	fcce61e3          	bltu	t3,a2,a28caa <memcpy+0x258>
  a28cec:	b3dd                	j	a28ad2 <memcpy+0x80>

00a28cee <memcmp>:
  a28cee:	962a                	add	a2,a2,a0
  a28cf0:	00c50c63          	beq	a0,a2,a28d08 <memcmp+0x1a>
  a28cf4:	211c                	lbu	a5,0(a0)
  a28cf6:	2198                	lbu	a4,0(a1)
  a28cf8:	00e78563          	beq	a5,a4,a28d02 <memcmp+0x14>
  a28cfc:	40e78533          	sub	a0,a5,a4
  a28d00:	8082                	ret
  a28d02:	0505                	addi	a0,a0,1
  a28d04:	0585                	addi	a1,a1,1
  a28d06:	b7ed                	j	a28cf0 <memcmp+0x2>
  a28d08:	4501                	li	a0,0
  a28d0a:	8082                	ret

00a28d0c <strlen>:
  a28d0c:	87aa                	mv	a5,a0
  a28d0e:	0037f713          	andi	a4,a5,3
  a28d12:	e31d                	bnez	a4,a28d38 <strlen+0x2c>
  a28d14:	fefe feff 059f      	l.li	a1,0xfefefeff
  a28d1a:	8080 8080 061f      	l.li	a2,0x80808080
  a28d20:	4394                	lw	a3,0(a5)
  a28d22:	00b68733          	add	a4,a3,a1
  a28d26:	fff6c693          	not	a3,a3
  a28d2a:	8f75                	and	a4,a4,a3
  a28d2c:	8f71                	and	a4,a4,a2
  a28d2e:	cf01                	beqz	a4,a28d46 <strlen+0x3a>
  a28d30:	2398                	lbu	a4,0(a5)
  a28d32:	c709                	beqz	a4,a28d3c <strlen+0x30>
  a28d34:	0785                	addi	a5,a5,1
  a28d36:	bfed                	j	a28d30 <strlen+0x24>
  a28d38:	2398                	lbu	a4,0(a5)
  a28d3a:	e701                	bnez	a4,a28d42 <strlen+0x36>
  a28d3c:	40a78533          	sub	a0,a5,a0
  a28d40:	8082                	ret
  a28d42:	0785                	addi	a5,a5,1
  a28d44:	b7e9                	j	a28d0e <strlen+0x2>
  a28d46:	0791                	addi	a5,a5,4
  a28d48:	bfe1                	j	a28d20 <strlen+0x14>

00a28d4a <memmove>:
  a28d4a:	882a                	mv	a6,a0
  a28d4c:	872e                	mv	a4,a1
  a28d4e:	87b2                	mv	a5,a2
  a28d50:	02b50f63          	beq	a0,a1,a28d8e <memmove+0x44>
  a28d54:	56f9                	li	a3,-2
  a28d56:	02d606b3          	mul	a3,a2,a3
  a28d5a:	40a588b3          	sub	a7,a1,a0
  a28d5e:	40c888b3          	sub	a7,a7,a2
  a28d62:	0116e363          	bltu	a3,a7,a28d68 <memmove+0x1e>
  a28d66:	b1f5                	j	a28a52 <memcpy>
  a28d68:	00a5c6b3          	xor	a3,a1,a0
  a28d6c:	8a8d                	andi	a3,a3,3
  a28d6e:	04b57863          	bgeu	a0,a1,a28dbe <memmove+0x74>
  a28d72:	c6a1                	beqz	a3,a28dba <memmove+0x70>
  a28d74:	86aa                	mv	a3,a0
  a28d76:	97b6                	add	a5,a5,a3
  a28d78:	00f68a63          	beq	a3,a5,a28d8c <memmove+0x42>
  a28d7c:	0705                	addi	a4,a4,1
  a28d7e:	fff74603          	lbu	a2,-1(a4)
  a28d82:	0685                	addi	a3,a3,1
  a28d84:	fec68fa3          	sb	a2,-1(a3)
  a28d88:	bfc5                	j	a28d78 <memmove+0x2e>
  a28d8a:	e781                	bnez	a5,a28d92 <memmove+0x48>
  a28d8c:	8742                	mv	a4,a6
  a28d8e:	853a                	mv	a0,a4
  a28d90:	8082                	ret
  a28d92:	0705                	addi	a4,a4,1
  a28d94:	fff74603          	lbu	a2,-1(a4)
  a28d98:	0685                	addi	a3,a3,1
  a28d9a:	17fd                	addi	a5,a5,-1
  a28d9c:	fec68fa3          	sb	a2,-1(a3)
  a28da0:	0036f613          	andi	a2,a3,3
  a28da4:	f27d                	bnez	a2,a28d8a <memmove+0x40>
  a28da6:	460d                	li	a2,3
  a28da8:	fcf677e3          	bgeu	a2,a5,a28d76 <memmove+0x2c>
  a28dac:	430c                	lw	a1,0(a4)
  a28dae:	17f1                	addi	a5,a5,-4
  a28db0:	0691                	addi	a3,a3,4
  a28db2:	feb6ae23          	sw	a1,-4(a3)
  a28db6:	0711                	addi	a4,a4,4
  a28db8:	bfc5                	j	a28da8 <memmove+0x5e>
  a28dba:	86aa                	mv	a3,a0
  a28dbc:	b7d5                	j	a28da0 <memmove+0x56>
  a28dbe:	ee81                	bnez	a3,a28dd6 <memmove+0x8c>
  a28dc0:	962a                	add	a2,a2,a0
  a28dc2:	00f586b3          	add	a3,a1,a5
  a28dc6:	00367593          	andi	a1,a2,3
  a28dca:	40e687b3          	sub	a5,a3,a4
  a28dce:	ed89                	bnez	a1,a28de8 <memmove+0x9e>
  a28dd0:	468d                	li	a3,3
  a28dd2:	02f6e363          	bltu	a3,a5,a28df8 <memmove+0xae>
  a28dd6:	dbdd                	beqz	a5,a28d8c <memmove+0x42>
  a28dd8:	17fd                	addi	a5,a5,-1
  a28dda:	00f706b3          	add	a3,a4,a5
  a28dde:	2290                	lbu	a2,0(a3)
  a28de0:	00f806b3          	add	a3,a6,a5
  a28de4:	a290                	sb	a2,0(a3)
  a28de6:	bfc5                	j	a28dd6 <memmove+0x8c>
  a28de8:	167d                	addi	a2,a2,-1
  a28dea:	fad701e3          	beq	a4,a3,a28d8c <memmove+0x42>
  a28dee:	fff6c783          	lbu	a5,-1(a3)
  a28df2:	16fd                	addi	a3,a3,-1
  a28df4:	a21c                	sb	a5,0(a2)
  a28df6:	bfc1                	j	a28dc6 <memmove+0x7c>
  a28df8:	17f1                	addi	a5,a5,-4
  a28dfa:	00f70633          	add	a2,a4,a5
  a28dfe:	420c                	lw	a1,0(a2)
  a28e00:	00f80633          	add	a2,a6,a5
  a28e04:	c20c                	sw	a1,0(a2)
  a28e06:	b7f1                	j	a28dd2 <memmove+0x88>

00a28e08 <reset>:
  a28e08:	8018                	push	{ra},-16
  a28e0a:	5c1030ef          	jal	ra,a2cbca <update_reset_count>
  a28e0e:	400027b7          	lui	a5,0x40002
  a28e12:	4711                	li	a4,4
  a28e14:	10e79823          	sh	a4,272(a5) # 40002110 <_gp_+0x3f5cd954>
  a28e18:	a001                	j	a28e18 <reset+0x10>

00a28e1a <boot_fail>:
  a28e1a:	a001                	j	a28e1a <boot_fail>

00a28e1c <boot_regist_flash_cmd>:
  a28e1c:	8018                	push	{ra},-16
  a28e1e:	862a                	mv	a2,a0
  a28e20:	46d1                	li	a3,20
  a28e22:	45d1                	li	a1,20
  a28e24:	00a0 5500 051f      	l.li	a0,0xa05500
  a28e2a:	347070ef          	jal	ra,a30970 <memcpy_s>
  a28e2e:	c501                	beqz	a0,a28e36 <boot_regist_flash_cmd+0x1a>
  a28e30:	80000537          	lui	a0,0x80000
  a28e34:	0511                	addi	a0,a0,4 # 80000004 <_gp_+0x7f5cb848>
  a28e36:	8014                	popret	{ra},16

00a28e38 <jump_to_execute_addr>:
  a28e38:	8018                	push	{ra},-16
  a28e3a:	9502                	jalr	a0
  a28e3c:	8010                	pop	{ra},16
  a28e3e:	bff1                	j	a28e1a <boot_fail>

00a28e40 <ws63_set_try_fix_cnt>:
  a28e40:	40000737          	lui	a4,0x40000
  a28e44:	471c                	lw	a5,8(a4)
  a28e46:	893d                	andi	a0,a0,15
  a28e48:	9bc1                	andi	a5,a5,-16
  a28e4a:	8fc9                	or	a5,a5,a0
  a28e4c:	c71c                	sw	a5,8(a4)
  a28e4e:	8082                	ret

00a28e50 <sfc_flash_erase>:
  a28e50:	ffe007b7          	lui	a5,0xffe00
  a28e54:	953e                	add	a0,a0,a5
  a28e56:	10d0306f          	j	a2c762 <uapi_sfc_reg_erase>

00a28e5a <sfc_flash_write>:
  a28e5a:	87b2                	mv	a5,a2
  a28e5c:	862e                	mv	a2,a1
  a28e5e:	85be                	mv	a1,a5
  a28e60:	0050306f          	j	a2c664 <uapi_sfc_reg_write>

00a28e64 <sfc_flash_read>:
  a28e64:	87b2                	mv	a5,a2
  a28e66:	862e                	mv	a2,a1
  a28e68:	85be                	mv	a1,a5
  a28e6a:	7260306f          	j	a2c590 <uapi_sfc_reg_read>

00a28e6e <sfc_flash_init>:
  a28e6e:	00a3 1d20 051f      	l.li	a0,0xa31d20
  a28e74:	6720306f          	j	a2c4e6 <uapi_sfc_init>

00a28e78 <start_fastboot>:
  a28e78:	8a58                	push	{ra,s0-s3},-192
  a28e7a:	40003737          	lui	a4,0x40003
  a28e7e:	ce02                	sw	zero,28(sp)
  a28e80:	d002                	sw	zero,32(sp)
  a28e82:	d202                	sw	zero,36(sp)
  a28e84:	40872783          	lw	a5,1032(a4) # 40003408 <_gp_+0x3f5cec4c>
  a28e88:	0387e793          	ori	a5,a5,56
  a28e8c:	40f72423          	sw	a5,1032(a4)
  a28e90:	2cd030ef          	jal	ra,a2c95c <boot_clock_adapt>
  a28e94:	451d                	li	a0,7
  a28e96:	03b010ef          	jal	ra,a2a6d0 <uapi_watchdog_init>
  a28e9a:	4501                	li	a0,0
  a28e9c:	087010ef          	jal	ra,a2a722 <uapi_watchdog_enable>
  a28ea0:	0001 c200 079f      	l.li	a5,0x1c200
  a28ea6:	d43e                	sw	a5,40(sp)
  a28ea8:	10800793          	li	a5,264
  a28eac:	d63e                	sw	a5,44(sp)
  a28eae:	0004 0102 079f      	l.li	a5,0x40102
  a28eb4:	d83e                	sw	a5,48(sp)
  a28eb6:	328010ef          	jal	ra,a2a1de <uapi_tcxo_init>
  a28eba:	4631                	li	a2,12
  a28ebc:	102c                	addi	a1,sp,40
  a28ebe:	850a                	mv	a0,sp
  a28ec0:	3e49                	jal	ra,a28a52 <memcpy>
  a28ec2:	4581                	li	a1,0
  a28ec4:	850a                	mv	a0,sp
  a28ec6:	475030ef          	jal	ra,a2cb3a <hiburn_uart_init>
  a28eca:	00a3 1d2c 051f      	l.li	a0,0xa31d2c
  a28ed0:	40b030ef          	jal	ra,a2cada <boot_msg0>
  a28ed4:	463040ef          	jal	ra,a2db36 <uapi_partition_init>
  a28ed8:	0f2030ef          	jal	ra,a2bfca <pmp_enable>
  a28edc:	21b030ef          	jal	ra,a2c8f6 <malloc_port_init>
  a28ee0:	00a3 1d48 051f      	l.li	a0,0xa31d48
  a28ee6:	3f5030ef          	jal	ra,a2cada <boot_msg0>
  a28eea:	00a2 8e6e 079f      	l.li	a5,0xa28e6e
  a28ef0:	da3e                	sw	a5,52(sp)
  a28ef2:	00a2 8e64 079f      	l.li	a5,0xa28e64
  a28ef8:	dc3e                	sw	a5,56(sp)
  a28efa:	00a2 8e5a 079f      	l.li	a5,0xa28e5a
  a28f00:	de3e                	sw	a5,60(sp)
  a28f02:	1848                	addi	a0,sp,52
  a28f04:	00a2 8e50 079f      	l.li	a5,0xa28e50
  a28f0a:	c0be                	sw	a5,64(sp)
  a28f0c:	c282                	sw	zero,68(sp)
  a28f0e:	3739                	jal	ra,a28e1c <boot_regist_flash_cmd>
  a28f10:	3fb9                	jal	ra,a28e6e <sfc_flash_init>
  a28f12:	18050863          	beqz	a0,a290a2 <start_fastboot+0x22a>
  a28f16:	85aa                	mv	a1,a0
  a28f18:	00a3 1d64 051f      	l.li	a0,0xa31d64
  a28f1e:	3cf030ef          	jal	ra,a2caec <boot_msg1>
  a28f22:	27f030ef          	jal	ra,a2c9a0 <switch_flash_clock_to_pll>
  a28f26:	2a7030ef          	jal	ra,a2c9cc <config_sfc_ctrl_ds>
  a28f2a:	3ca030ef          	jal	ra,a2c2f4 <sfc_port_fix_sr>
  a28f2e:	85aa                	mv	a1,a0
  a28f30:	c511                	beqz	a0,a28f3c <start_fastboot+0xc4>
  a28f32:	00a3 1d90 051f      	l.li	a0,0xa31d90
  a28f38:	3b5030ef          	jal	ra,a2caec <boot_msg1>
  a28f3c:	00a3 1da4 059f      	l.li	a1,0xa31da4
  a28f42:	00a3 1db0 051f      	l.li	a0,0xa31db0
  a28f48:	0fc030ef          	jal	ra,a2c044 <print_str>
  a28f4c:	191030ef          	jal	ra,a2c8dc <set_efuse_period>
  a28f50:	0b3010ef          	jal	ra,a2a802 <uapi_efuse_init>
  a28f54:	5b6040ef          	jal	ra,a2d50a <ws63_upg_init>
  a28f58:	102c                	addi	a1,sp,40
  a28f5a:	02100513          	li	a0,33
  a28f5e:	cc02                	sw	zero,24(sp)
  a28f60:	455040ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a28f64:	e509                	bnez	a0,a28f6e <start_fastboot+0xf6>
  a28f66:	02814783          	lbu	a5,40(sp)
  a28f6a:	14078263          	beqz	a5,a290ae <start_fastboot+0x236>
  a28f6e:	0f000593          	li	a1,240
  a28f72:	00a3 1dcc 051f      	l.li	a0,0xa31dcc
  a28f78:	375030ef          	jal	ra,a2caec <boot_msg1>
  a28f7c:	086c                	addi	a1,sp,28
  a28f7e:	02000513          	li	a0,32
  a28f82:	433040ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a28f86:	c519                	beqz	a0,a28f94 <start_fastboot+0x11c>
  a28f88:	00a3 1e80 051f      	l.li	a0,0xa31e80
  a28f8e:	34d030ef          	jal	ra,a2cada <boot_msg0>
  a28f92:	3d9d                	jal	ra,a28e08 <reset>
  a28f94:	5402                	lw	s0,32(sp)
  a28f96:	002007b7          	lui	a5,0x200
  a28f9a:	10078993          	addi	s3,a5,256 # 200100 <__heap_size+0x1e2b08>
  a28f9e:	99a2                	add	s3,s3,s0
  a28fa0:	da02                	sw	zero,52(sp)
  a28fa2:	dc02                	sw	zero,56(sp)
  a28fa4:	de02                	sw	zero,60(sp)
  a28fa6:	c082                	sw	zero,64(sp)
  a28fa8:	5912                	lw	s2,36(sp)
  a28faa:	0489a703          	lw	a4,72(s3)
  a28fae:	00f404b3          	add	s1,s0,a5
  a28fb2:	30078793          	addi	a5,a5,768
  a28fb6:	943e                	add	s0,s0,a5
  a28fb8:	3c78 96e1 079f      	l.li	a5,0x3c7896e1
  a28fbe:	9926                	add	s2,s2,s1
  a28fc0:	18f71163          	bne	a4,a5,a29142 <start_fastboot+0x2ca>
  a28fc4:	00a3 1eb4 051f      	l.li	a0,0xa31eb4
  a28fca:	311030ef          	jal	ra,a2cada <boot_msg0>
  a28fce:	00a2 8080 059f      	l.li	a1,0xa28080
  a28fd4:	8626                	mv	a2,s1
  a28fd6:	4519                	li	a0,6
  a28fd8:	29a1                	jal	ra,a29430 <verify_image_head>
  a28fda:	85aa                	mv	a1,a0
  a28fdc:	1e050d63          	beqz	a0,a291d6 <start_fastboot+0x35e>
  a28fe0:	00a3 1fc4 051f      	l.li	a0,0xa31fc4
  a28fe6:	307030ef          	jal	ra,a2caec <boot_msg1>
  a28fea:	400007b7          	lui	a5,0x40000
  a28fee:	4788                	lw	a0,8(a5)
  a28ff0:	893d                	andi	a0,a0,15
  a28ff2:	478d                	li	a5,3
  a28ff4:	1ef57b63          	bgeu	a0,a5,a291ea <start_fastboot+0x372>
  a28ff8:	0505                	addi	a0,a0,1
  a28ffa:	3599                	jal	ra,a28e40 <ws63_set_try_fix_cnt>
  a28ffc:	3531                	jal	ra,a28e08 <reset>
  a28ffe:	4501                	li	a0,0
  a29000:	3b7030ef          	jal	ra,a2cbb6 <set_reset_count>
  a29004:	4501                	li	a0,0
  a29006:	3d2d                	jal	ra,a28e40 <ws63_set_try_fix_cnt>
  a29008:	4671                	li	a2,28
  a2900a:	4581                	li	a1,0
  a2900c:	1848                	addi	a0,sp,52
  a2900e:	3a59                	jal	ra,a289a4 <memset>
  a29010:	1848                	addi	a0,sp,52
  a29012:	0db040ef          	jal	ra,a2d8ec <mfg_get_ftm_run_region>
  a29016:	c90d                	beqz	a0,a29048 <start_fastboot+0x1d0>
  a29018:	57e2                	lw	a5,56(sp)
  a2901a:	4b7c f333 071f      	l.li	a4,0x4b7cf333
  a29020:	4394                	lw	a3,0(a5)
  a29022:	02e69363          	bne	a3,a4,a29048 <start_fastboot+0x1d0>
  a29026:	4726                	lw	a4,72(sp)
  a29028:	e305                	bnez	a4,a29048 <start_fastboot+0x1d0>
  a2902a:	4706                	lw	a4,64(sp)
  a2902c:	00c4d693          	srli	a3,s1,0xc
  a29030:	83b1                	srli	a5,a5,0xc
  a29032:	94ba                	add	s1,s1,a4
  a29034:	44008737          	lui	a4,0x44008
  a29038:	80d72023          	sw	a3,-2048(a4) # 44007800 <_gp_+0x435d3044>
  a2903c:	80b1                	srli	s1,s1,0xc
  a2903e:	82972023          	sw	s1,-2016(a4)
  a29042:	8f95                	sub	a5,a5,a3
  a29044:	84f72023          	sw	a5,-1984(a4)
  a29048:	400007b7          	lui	a5,0x40000
  a2904c:	53d8                	lw	a4,36(a5)
  a2904e:	0207a223          	sw	zero,36(a5) # 40000024 <_gp_+0x3f5cb868>
  a29052:	5a5a 5a5a 079f      	l.li	a5,0x5a5a5a5a
  a29058:	04f71063          	bne	a4,a5,a29098 <start_fastboot+0x220>
  a2905c:	77a010ef          	jal	ra,a2a7d6 <uapi_watchdog_kick>
  a29060:	00a3 208c 051f      	l.li	a0,0xa3208c
  a29066:	275030ef          	jal	ra,a2cada <boot_msg0>
  a2906a:	102c                	addi	a1,sp,40
  a2906c:	4509                	li	a0,2
  a2906e:	d402                	sw	zero,40(sp)
  a29070:	d602                	sw	zero,44(sp)
  a29072:	d802                	sw	zero,48(sp)
  a29074:	da02                	sw	zero,52(sp)
  a29076:	dc02                	sw	zero,56(sp)
  a29078:	de02                	sw	zero,60(sp)
  a2907a:	33b040ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2907e:	84aa                	mv	s1,a0
  a29080:	184c                	addi	a1,sp,52
  a29082:	4505                	li	a0,1
  a29084:	331040ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a29088:	8d45                	or	a0,a0,s1
  a2908a:	18050c63          	beqz	a0,a29222 <start_fastboot+0x3aa>
  a2908e:	00a3 20ac 051f      	l.li	a0,0xa320ac
  a29094:	247030ef          	jal	ra,a2cada <boot_msg0>
  a29098:	73e010ef          	jal	ra,a2a7d6 <uapi_watchdog_kick>
  a2909c:	8522                	mv	a0,s0
  a2909e:	3b69                	jal	ra,a28e38 <jump_to_execute_addr>
  a290a0:	8a54                	popret	{ra,s0-s3},192
  a290a2:	00a3 1d7c 051f      	l.li	a0,0xa31d7c
  a290a8:	233030ef          	jal	ra,a2cada <boot_msg0>
  a290ac:	bd9d                	j	a28f22 <start_fastboot+0xaa>
  a290ae:	0828                	addi	a0,sp,24
  a290b0:	290040ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a290b4:	4562                	lw	a0,24(sp)
  a290b6:	06c00593          	li	a1,108
  a290ba:	1850                	addi	a2,sp,52
  a290bc:	31e040ef          	jal	ra,a2d3da <upg_flash_read>
  a290c0:	85aa                	mv	a1,a0
  a290c2:	c509                	beqz	a0,a290cc <start_fastboot+0x254>
  a290c4:	00a3 1dec 051f      	l.li	a0,0xa31dec
  a290ca:	b57d                	j	a28f78 <start_fastboot+0x100>
  a290cc:	5752                	lw	a4,52(sp)
  a290ce:	55aa 55aa 079f      	l.li	a5,0x55aa55aa
  a290d4:	00f71a63          	bne	a4,a5,a290e8 <start_fastboot+0x270>
  a290d8:	477a                	lw	a4,156(sp)
  a290da:	aa55 aa55 079f      	l.li	a5,0xaa55aa55
  a290e0:	00f71463          	bne	a4,a5,a290e8 <start_fastboot+0x270>
  a290e4:	47ea                	lw	a5,152(sp)
  a290e6:	e799                	bnez	a5,a290f4 <start_fastboot+0x27c>
  a290e8:	00a3 1e04 051f      	l.li	a0,0xa31e04
  a290ee:	1ed030ef          	jal	ra,a2cada <boot_msg0>
  a290f2:	b569                	j	a28f7c <start_fastboot+0x104>
  a290f4:	00a3 1e1c 051f      	l.li	a0,0xa31e1c
  a290fa:	1e1030ef          	jal	ra,a2cada <boot_msg0>
  a290fe:	21d050ef          	jal	ra,a2eb1a <uapi_upg_start>
  a29102:	800037b7          	lui	a5,0x80003
  a29106:	04778693          	addi	a3,a5,71 # 80003047 <_gp_+0x7f5ce88b>
  a2910a:	00a32737          	lui	a4,0xa32
  a2910e:	02d50263          	beq	a0,a3,a29132 <start_fastboot+0x2ba>
  a29112:	04078793          	addi	a5,a5,64
  a29116:	00f50e63          	beq	a0,a5,a29132 <start_fastboot+0x2ba>
  a2911a:	cd01                	beqz	a0,a29132 <start_fastboot+0x2ba>
  a2911c:	e2c70513          	addi	a0,a4,-468 # a31e2c <sfc_cfg+0x10c>
  a29120:	1bb030ef          	jal	ra,a2cada <boot_msg0>
  a29124:	00a3 1e48 051f      	l.li	a0,0xa31e48
  a2912a:	1b1030ef          	jal	ra,a2cada <boot_msg0>
  a2912e:	39e9                	jal	ra,a28e08 <reset>
  a29130:	b5b1                	j	a28f7c <start_fastboot+0x104>
  a29132:	e2c70513          	addi	a0,a4,-468
  a29136:	1a5030ef          	jal	ra,a2cada <boot_msg0>
  a2913a:	00a3 1e64 051f      	l.li	a0,0xa31e64
  a29140:	b7ed                	j	a2912a <start_fastboot+0x2b2>
  a29142:	012467b3          	or	a5,s0,s2
  a29146:	0ff7f793          	andi	a5,a5,255
  a2914a:	cb89                	beqz	a5,a2915c <start_fastboot+0x2e4>
  a2914c:	864a                	mv	a2,s2
  a2914e:	85a6                	mv	a1,s1
  a29150:	00a3 1ecc 051f      	l.li	a0,0xa31ecc
  a29156:	1b3030ef          	jal	ra,a2cb08 <boot_msg2>
  a2915a:	317d                	jal	ra,a28e08 <reset>
  a2915c:	00a3 1f08 051f      	l.li	a0,0xa31f08
  a29162:	179030ef          	jal	ra,a2cada <boot_msg0>
  a29166:	04c98793          	addi	a5,s3,76
  a2916a:	dc3e                	sw	a5,56(sp)
  a2916c:	47f1                	li	a5,28
  a2916e:	184c                	addi	a1,sp,52
  a29170:	de3e                	sw	a5,60(sp)
  a29172:	4641                	li	a2,16
  a29174:	4785                	li	a5,1
  a29176:	850a                	mv	a0,sp
  a29178:	04f10023          	sb	a5,64(sp)
  a2917c:	38d9                	jal	ra,a28a52 <memcpy>
  a2917e:	850a                	mv	a0,sp
  a29180:	1a1000ef          	jal	ra,a29b20 <drv_rom_cipher_config_odrk1>
  a29184:	85aa                	mv	a1,a0
  a29186:	c519                	beqz	a0,a29194 <start_fastboot+0x31c>
  a29188:	00a3 1f20 051f      	l.li	a0,0xa31f20
  a2918e:	15f030ef          	jal	ra,a2caec <boot_msg1>
  a29192:	399d                	jal	ra,a28e08 <reset>
  a29194:	85a2                	mv	a1,s0
  a29196:	4741                	li	a4,16
  a29198:	06c98693          	addi	a3,s3,108
  a2919c:	864a                	mv	a2,s2
  a2919e:	4501                	li	a0,0
  a291a0:	1e5000ef          	jal	ra,a29b84 <drv_rom_cipher_fapc_config>
  a291a4:	85aa                	mv	a1,a0
  a291a6:	c519                	beqz	a0,a291b4 <start_fastboot+0x33c>
  a291a8:	00a3 1f54 051f      	l.li	a0,0xa31f54
  a291ae:	13f030ef          	jal	ra,a2caec <boot_msg1>
  a291b2:	3999                	jal	ra,a28e08 <reset>
  a291b4:	85ca                	mv	a1,s2
  a291b6:	4685                	li	a3,1
  a291b8:	00a00637          	lui	a2,0xa00
  a291bc:	4505                	li	a0,1
  a291be:	20d000ef          	jal	ra,a29bca <drv_rom_cipher_fapc_bypass_config>
  a291c2:	85aa                	mv	a1,a0
  a291c4:	e00505e3          	beqz	a0,a28fce <start_fastboot+0x156>
  a291c8:	00a3 1f88 051f      	l.li	a0,0xa31f88
  a291ce:	11f030ef          	jal	ra,a2caec <boot_msg1>
  a291d2:	391d                	jal	ra,a28e08 <reset>
  a291d4:	bbed                	j	a28fce <start_fastboot+0x156>
  a291d6:	85a2                	mv	a1,s0
  a291d8:	8526                	mv	a0,s1
  a291da:	290d                	jal	ra,a2960c <verify_image_body>
  a291dc:	85aa                	mv	a1,a0
  a291de:	e20500e3          	beqz	a0,a28ffe <start_fastboot+0x186>
  a291e2:	00a3 1ff4 051f      	l.li	a0,0xa31ff4
  a291e8:	bbfd                	j	a28fe6 <start_fastboot+0x16e>
  a291ea:	4799                	li	a5,6
  a291ec:	e0f578e3          	bgeu	a0,a5,a28ffc <start_fastboot+0x184>
  a291f0:	0505                	addi	a0,a0,1
  a291f2:	31b9                	jal	ra,a28e40 <ws63_set_try_fix_cnt>
  a291f4:	406050ef          	jal	ra,a2e5fa <uapi_upg_reset_upgrade_flag>
  a291f8:	85aa                	mv	a1,a0
  a291fa:	c519                	beqz	a0,a29208 <start_fastboot+0x390>
  a291fc:	00a3 201c 051f      	l.li	a0,0xa3201c
  a29202:	0eb030ef          	jal	ra,a2caec <boot_msg1>
  a29206:	bbdd                	j	a28ffc <start_fastboot+0x184>
  a29208:	468050ef          	jal	ra,a2e670 <uapi_upg_request_upgrade>
  a2920c:	c519                	beqz	a0,a2921a <start_fastboot+0x3a2>
  a2920e:	00a3 203c 051f      	l.li	a0,0xa3203c
  a29214:	0c7030ef          	jal	ra,a2cada <boot_msg0>
  a29218:	b3d5                	j	a28ffc <start_fastboot+0x184>
  a2921a:	00a3 2068 051f      	l.li	a0,0xa32068
  a29220:	bfd5                	j	a29214 <start_fastboot+0x39c>
  a29222:	55f2                	lw	a1,60(sp)
  a29224:	5562                	lw	a0,56(sp)
  a29226:	53c030ef          	jal	ra,a2c762 <uapi_sfc_reg_erase>
  a2922a:	85aa                	mv	a1,a0
  a2922c:	c511                	beqz	a0,a29238 <start_fastboot+0x3c0>
  a2922e:	00a3 20d0 051f      	l.li	a0,0xa320d0
  a29234:	0b9030ef          	jal	ra,a2caec <boot_msg1>
  a29238:	57b2                	lw	a5,44(sp)
  a2923a:	5642                	lw	a2,48(sp)
  a2923c:	5562                	lw	a0,56(sp)
  a2923e:	002005b7          	lui	a1,0x200
  a29242:	95be                	add	a1,a1,a5
  a29244:	420030ef          	jal	ra,a2c664 <uapi_sfc_reg_write>
  a29248:	85aa                	mv	a1,a0
  a2924a:	c511                	beqz	a0,a29256 <start_fastboot+0x3de>
  a2924c:	00a3 20fc 051f      	l.li	a0,0xa320fc
  a29252:	09b030ef          	jal	ra,a2caec <boot_msg1>
  a29256:	00a3 2128 051f      	l.li	a0,0xa32128
  a2925c:	bd25                	j	a29094 <start_fastboot+0x21c>

00a2925e <check_image_id>:
  a2925e:	00a326b7          	lui	a3,0xa32
  a29262:	1f068713          	addi	a4,a3,496 # a321f0 <g_verify_table>
  a29266:	4781                	li	a5,0
  a29268:	1f068693          	addi	a3,a3,496
  a2926c:	4839                	li	a6,14
  a2926e:	00074883          	lbu	a7,0(a4)
  a29272:	02a89463          	bne	a7,a0,a2929a <check_image_id+0x3c>
  a29276:	00174883          	lbu	a7,1(a4)
  a2927a:	02b89063          	bne	a7,a1,a2929a <check_image_id+0x3c>
  a2927e:	0cf697db          	muliadd	a5,a3,a5,12
  a29282:	43dc                	lw	a5,4(a5)
  a29284:	4501                	li	a0,0
  a29286:	02c78063          	beq	a5,a2,a292a6 <check_image_id+0x48>
  a2928a:	8018                	push	{ra},-16
  a2928c:	00a3 2170 051f      	l.li	a0,0xa32170
  a29292:	049030ef          	jal	ra,a2cada <boot_msg0>
  a29296:	557d                	li	a0,-1
  a29298:	8014                	popret	{ra},16
  a2929a:	0785                	addi	a5,a5,1
  a2929c:	0731                	addi	a4,a4,12
  a2929e:	fd0798e3          	bne	a5,a6,a2926e <check_image_id+0x10>
  a292a2:	4781                	li	a5,0
  a292a4:	b7c5                	j	a29284 <check_image_id+0x26>
  a292a6:	8082                	ret

00a292a8 <check_verify_enable>:
  a292a8:	8118                	push	{ra},-32
  a292aa:	57fd                	li	a5,-1
  a292ac:	4605                	li	a2,1
  a292ae:	00f10593          	addi	a1,sp,15
  a292b2:	451d                	li	a0,7
  a292b4:	00f107a3          	sb	a5,15(sp)
  a292b8:	616030ef          	jal	ra,a2c8ce <efuse_read_item>
  a292bc:	c901                	beqz	a0,a292cc <check_verify_enable+0x24>
  a292be:	00a3 21b4 051f      	l.li	a0,0xa321b4
  a292c4:	017030ef          	jal	ra,a2cada <boot_msg0>
  a292c8:	557d                	li	a0,-1
  a292ca:	8114                	popret	{ra},32
  a292cc:	00f14783          	lbu	a5,15(sp)
  a292d0:	4501                	li	a0,0
  a292d2:	ffe5                	bnez	a5,a292ca <check_verify_enable+0x22>
  a292d4:	80000537          	lui	a0,0x80000
  a292d8:	0509                	addi	a0,a0,2 # 80000002 <_gp_+0x7f5cb846>
  a292da:	bfc5                	j	a292ca <check_verify_enable+0x22>

00a292dc <check_msid>:
  a292dc:	8138                	push	{ra,s0-s1},-32
  a292de:	842a                	mv	s0,a0
  a292e0:	84ae                	mv	s1,a1
  a292e2:	4611                	li	a2,4
  a292e4:	006c                	addi	a1,sp,12
  a292e6:	4519                	li	a0,6
  a292e8:	c602                	sw	zero,12(sp)
  a292ea:	5e4030ef          	jal	ra,a2c8ce <efuse_read_item>
  a292ee:	c901                	beqz	a0,a292fe <check_msid+0x22>
  a292f0:	00a3 2180 051f      	l.li	a0,0xa32180
  a292f6:	7e4030ef          	jal	ra,a2cada <boot_msg0>
  a292fa:	557d                	li	a0,-1
  a292fc:	8134                	popret	{ra,s0-s1},32
  a292fe:	47b2                	lw	a5,12(sp)
  a29300:	4501                	li	a0,0
  a29302:	8c3d                	xor	s0,s0,a5
  a29304:	8c65                	and	s0,s0,s1
  a29306:	d87d                	beqz	s0,a292fc <check_msid+0x20>
  a29308:	00a3 21a0 051f      	l.li	a0,0xa321a0
  a2930e:	b7e5                	j	a292f6 <check_msid+0x1a>

00a29310 <secure_authenticate.constprop.2>:
  a29310:	8448                	push	{ra,s0-s2},-80
  a29312:	02010913          	addi	s2,sp,32
  a29316:	84aa                	mv	s1,a0
  a29318:	8436                	mv	s0,a3
  a2931a:	852e                	mv	a0,a1
  a2931c:	02000693          	li	a3,32
  a29320:	85b2                	mv	a1,a2
  a29322:	864a                	mv	a2,s2
  a29324:	c402                	sw	zero,8(sp)
  a29326:	c602                	sw	zero,12(sp)
  a29328:	c802                	sw	zero,16(sp)
  a2932a:	c002                	sw	zero,0(sp)
  a2932c:	c202                	sw	zero,4(sp)
  a2932e:	ca02                	sw	zero,20(sp)
  a29330:	cc02                	sw	zero,24(sp)
  a29332:	ce02                	sw	zero,28(sp)
  a29334:	61f000ef          	jal	ra,a2a152 <drv_rom_cipher_sha256>
  a29338:	c909                	beqz	a0,a2934a <secure_authenticate.constprop.2+0x3a>
  a2933a:	00a3 2298 051f      	l.li	a0,0xa32298
  a29340:	79a030ef          	jal	ra,a2cada <boot_msg0>
  a29344:	57fd                	li	a5,-1
  a29346:	853e                	mv	a0,a5
  a29348:	8444                	popret	{ra,s0-s2},80
  a2934a:	02000793          	li	a5,32
  a2934e:	c426                	sw	s1,8(sp)
  a29350:	ca22                	sw	s0,20(sp)
  a29352:	02048493          	addi	s1,s1,32
  a29356:	02040413          	addi	s0,s0,32
  a2935a:	0850                	addi	a2,sp,20
  a2935c:	858a                	mv	a1,sp
  a2935e:	0028                	addi	a0,sp,8
  a29360:	c83e                	sw	a5,16(sp)
  a29362:	c03e                	sw	a5,0(sp)
  a29364:	ce3e                	sw	a5,28(sp)
  a29366:	c626                	sw	s1,12(sp)
  a29368:	c24a                	sw	s2,4(sp)
  a2936a:	cc22                	sw	s0,24(sp)
  a2936c:	5df000ef          	jal	ra,a2a14a <drv_rom_cipher_pke_bp256r_verify>
  a29370:	4781                	li	a5,0
  a29372:	d971                	beqz	a0,a29346 <secure_authenticate.constprop.2+0x36>
  a29374:	00a3 22c8 051f      	l.li	a0,0xa322c8
  a2937a:	b7d9                	j	a29340 <secure_authenticate.constprop.2+0x30>

00a2937c <check_die_id.constprop.3>:
  a2937c:	8138                	push	{ra,s0-s1},-32
  a2937e:	84aa                	mv	s1,a0
  a29380:	4641                	li	a2,16
  a29382:	858a                	mv	a1,sp
  a29384:	4505                	li	a0,1
  a29386:	c002                	sw	zero,0(sp)
  a29388:	c202                	sw	zero,4(sp)
  a2938a:	c402                	sw	zero,8(sp)
  a2938c:	c602                	sw	zero,12(sp)
  a2938e:	540030ef          	jal	ra,a2c8ce <efuse_read_item>
  a29392:	c909                	beqz	a0,a293a4 <check_die_id.constprop.3+0x28>
  a29394:	00a3 213c 051f      	l.li	a0,0xa3213c
  a2939a:	740030ef          	jal	ra,a2cada <boot_msg0>
  a2939e:	547d                	li	s0,-1
  a293a0:	8522                	mv	a0,s0
  a293a2:	8134                	popret	{ra,s0-s1},32
  a293a4:	842a                	mv	s0,a0
  a293a6:	4641                	li	a2,16
  a293a8:	85a6                	mv	a1,s1
  a293aa:	850a                	mv	a0,sp
  a293ac:	943ff0ef          	jal	ra,a28cee <memcmp>
  a293b0:	d965                	beqz	a0,a293a0 <check_die_id.constprop.3+0x24>
  a293b2:	00a3 215c 051f      	l.li	a0,0xa3215c
  a293b8:	b7cd                	j	a2939a <check_die_id.constprop.3+0x1e>

00a293ba <check_version>:
  a293ba:	8138                	push	{ra,s0-s1},-32
  a293bc:	00a32737          	lui	a4,0xa32
  a293c0:	84b2                	mv	s1,a2
  a293c2:	1f070693          	addi	a3,a4,496 # a321f0 <g_verify_table>
  a293c6:	842e                	mv	s0,a1
  a293c8:	c602                	sw	zero,12(sp)
  a293ca:	4781                	li	a5,0
  a293cc:	1f070713          	addi	a4,a4,496
  a293d0:	4639                	li	a2,14
  a293d2:	228c                	lbu	a1,0(a3)
  a293d4:	02a59463          	bne	a1,a0,a293fc <check_version+0x42>
  a293d8:	0cf717db          	muliadd	a5,a4,a5,12
  a293dc:	2788                	lbu	a0,8(a5)
  a293de:	006c                	addi	a1,sp,12
  a293e0:	4611                	li	a2,4
  a293e2:	4ec030ef          	jal	ra,a2c8ce <efuse_read_item>
  a293e6:	87aa                	mv	a5,a0
  a293e8:	009475b3          	and	a1,s0,s1
  a293ec:	c115                	beqz	a0,a29410 <check_version+0x56>
  a293ee:	00a3 2180 051f      	l.li	a0,0xa32180
  a293f4:	6e6030ef          	jal	ra,a2cada <boot_msg0>
  a293f8:	557d                	li	a0,-1
  a293fa:	8134                	popret	{ra,s0-s1},32
  a293fc:	0785                	addi	a5,a5,1
  a293fe:	06b1                	addi	a3,a3,12
  a29400:	fcc799e3          	bne	a5,a2,a293d2 <check_version+0x18>
  a29404:	4501                	li	a0,0
  a29406:	bfe1                	j	a293de <check_version+0x24>
  a29408:	fff58713          	addi	a4,a1,-1 # 1fffff <__heap_size+0x1e2a07>
  a2940c:	8df9                	and	a1,a1,a4
  a2940e:	0785                	addi	a5,a5,1
  a29410:	fde5                	bnez	a1,a29408 <check_version+0x4e>
  a29412:	4632                	lw	a2,12(sp)
  a29414:	8e65                	and	a2,a2,s1
  a29416:	ea01                	bnez	a2,a29426 <check_version+0x6c>
  a29418:	4501                	li	a0,0
  a2941a:	feb7f0e3          	bgeu	a5,a1,a293fa <check_version+0x40>
  a2941e:	00a3 21d8 051f      	l.li	a0,0xa321d8
  a29424:	bfc1                	j	a293f4 <check_version+0x3a>
  a29426:	fff60713          	addi	a4,a2,-1 # 9fffff <__heap_size+0x9e2a07>
  a2942a:	8e79                	and	a2,a2,a4
  a2942c:	0585                	addi	a1,a1,1
  a2942e:	b7e5                	j	a29416 <check_version+0x5c>

00a29430 <verify_image_head>:
  a29430:	8048                	push	{ra,s0-s2},-16
  a29432:	1c060963          	beqz	a2,a29604 <verify_image_head+0x1d4>
  a29436:	f0000793          	li	a5,-256
  a2943a:	1cf60563          	beq	a2,a5,a29604 <verify_image_head+0x1d4>
  a2943e:	8432                	mv	s0,a2
  a29440:	892e                	mv	s2,a1
  a29442:	84aa                	mv	s1,a0
  a29444:	3595                	jal	ra,a292a8 <check_verify_enable>
  a29446:	800007b7          	lui	a5,0x80000
  a2944a:	0789                	addi	a5,a5,2 # 80000002 <_gp_+0x7f5cb846>
  a2944c:	02f51563          	bne	a0,a5,a29476 <verify_image_head+0x46>
  a29450:	00a3 2380 051f      	l.li	a0,0xa32380
  a29456:	684030ef          	jal	ra,a2cada <boot_msg0>
  a2945a:	35b9                	jal	ra,a292a8 <check_verify_enable>
  a2945c:	800007b7          	lui	a5,0x80000
  a29460:	0789                	addi	a5,a5,2 # 80000002 <_gp_+0x7f5cb846>
  a29462:	0ef51e63          	bne	a0,a5,a2955e <verify_image_head+0x12e>
  a29466:	00a3 24f0 051f      	l.li	a0,0xa324f0
  a2946c:	66e030ef          	jal	ra,a2cada <boot_msg0>
  a29470:	4401                	li	s0,0
  a29472:	8522                	mv	a0,s0
  a29474:	8044                	popret	{ra,s0-s2},16
  a29476:	ff05183b          	bnei	a0,-1,a29496 <verify_image_head+0x66>
  a2947a:	00a3 23b0 051f      	l.li	a0,0xa323b0
  a29480:	65a030ef          	jal	ra,a2cada <boot_msg0>
  a29484:	8000 15c0 041f      	l.li	s0,0x800015c0
  a2948a:	00a3 24d4 051f      	l.li	a0,0xa324d4
  a29490:	64a030ef          	jal	ra,a2cada <boot_msg0>
  a29494:	bff9                	j	a29472 <verify_image_head+0x42>
  a29496:	c491                	beqz	s1,a294a2 <verify_image_head+0x72>
  a29498:	ffe48793          	addi	a5,s1,-2
  a2949c:	9f81                	uxtb	a5
  a2949e:	0207f8bb          	bgeui	a5,2,a294c0 <verify_image_head+0x90>
  a294a2:	00a0 5514 079f      	l.li	a5,0xa05514
  a294a8:	439c                	lw	a5,0(a5)
  a294aa:	050785bb          	beqi	a5,5,a294c0 <verify_image_head+0x90>
  a294ae:	00a3 23dc 051f      	l.li	a0,0xa323dc
  a294b4:	626030ef          	jal	ra,a2cada <boot_msg0>
  a294b8:	8000 15c1 041f      	l.li	s0,0x800015c1
  a294be:	b7f1                	j	a2948a <verify_image_head+0x5a>
  a294c0:	4010                	lw	a2,0(s0)
  a294c2:	4581                	li	a1,0
  a294c4:	8526                	mv	a0,s1
  a294c6:	3b61                	jal	ra,a2925e <check_image_id>
  a294c8:	c911                	beqz	a0,a294dc <verify_image_head+0xac>
  a294ca:	00a3 240c 051f      	l.li	a0,0xa3240c
  a294d0:	60a030ef          	jal	ra,a2cada <boot_msg0>
  a294d4:	8000 15c2 041f      	l.li	s0,0x800015c2
  a294da:	bf45                	j	a2948a <verify_image_head+0x5a>
  a294dc:	0c040693          	addi	a3,s0,192
  a294e0:	0c000613          	li	a2,192
  a294e4:	85a2                	mv	a1,s0
  a294e6:	854a                	mv	a0,s2
  a294e8:	3525                	jal	ra,a29310 <secure_authenticate.constprop.2>
  a294ea:	c911                	beqz	a0,a294fe <verify_image_head+0xce>
  a294ec:	00a3 2434 051f      	l.li	a0,0xa32434
  a294f2:	5e8030ef          	jal	ra,a2cada <boot_msg0>
  a294f6:	8000 15c3 041f      	l.li	s0,0x800015c3
  a294fc:	b779                	j	a2948a <verify_image_head+0x5a>
  a294fe:	cc99                	beqz	s1,a2951c <verify_image_head+0xec>
  a29500:	5410                	lw	a2,40(s0)
  a29502:	504c                	lw	a1,36(s0)
  a29504:	8526                	mv	a0,s1
  a29506:	3d55                	jal	ra,a293ba <check_version>
  a29508:	c911                	beqz	a0,a2951c <verify_image_head+0xec>
  a2950a:	00a3 2460 051f      	l.li	a0,0xa32460
  a29510:	5ca030ef          	jal	ra,a2cada <boot_msg0>
  a29514:	8000 15c4 041f      	l.li	s0,0x800015c4
  a2951a:	bf85                	j	a2948a <verify_image_head+0x5a>
  a2951c:	580c                	lw	a1,48(s0)
  a2951e:	5448                	lw	a0,44(s0)
  a29520:	3b75                	jal	ra,a292dc <check_msid>
  a29522:	c911                	beqz	a0,a29536 <verify_image_head+0x106>
  a29524:	00a3 248c 051f      	l.li	a0,0xa3248c
  a2952a:	5b0030ef          	jal	ra,a2cada <boot_msg0>
  a2952e:	8000 15c5 041f      	l.li	s0,0x800015c5
  a29534:	bf99                	j	a2948a <verify_image_head+0x5a>
  a29536:	5858                	lw	a4,52(s0)
  a29538:	2a13 c812 079f      	l.li	a5,0x2a13c812
  a2953e:	f0f71ee3          	bne	a4,a5,a2945a <verify_image_head+0x2a>
  a29542:	03840513          	addi	a0,s0,56
  a29546:	3d1d                	jal	ra,a2937c <check_die_id.constprop.3>
  a29548:	f00509e3          	beqz	a0,a2945a <verify_image_head+0x2a>
  a2954c:	00a3 24b0 051f      	l.li	a0,0xa324b0
  a29552:	588030ef          	jal	ra,a2cada <boot_msg0>
  a29556:	8000 15c6 041f      	l.li	s0,0x800015c6
  a2955c:	b73d                	j	a2948a <verify_image_head+0x5a>
  a2955e:	ff05173b          	bnei	a0,-1,a2957a <verify_image_head+0x14a>
  a29562:	00a3 2520 051f      	l.li	a0,0xa32520
  a29568:	572030ef          	jal	ra,a2cada <boot_msg0>
  a2956c:	8000 15c0 041f      	l.li	s0,0x800015c0
  a29572:	00a3 25f0 051f      	l.li	a0,0xa325f0
  a29578:	bf21                	j	a29490 <verify_image_head+0x60>
  a2957a:	10042603          	lw	a2,256(s0)
  a2957e:	4585                	li	a1,1
  a29580:	8526                	mv	a0,s1
  a29582:	10040913          	addi	s2,s0,256
  a29586:	39e1                	jal	ra,a2925e <check_image_id>
  a29588:	c911                	beqz	a0,a2959c <verify_image_head+0x16c>
  a2958a:	00a3 254c 051f      	l.li	a0,0xa3254c
  a29590:	54a030ef          	jal	ra,a2cada <boot_msg0>
  a29594:	8000 15c2 041f      	l.li	s0,0x800015c2
  a2959a:	bfe1                	j	a29572 <verify_image_head+0x142>
  a2959c:	28040693          	addi	a3,s0,640
  a295a0:	18000613          	li	a2,384
  a295a4:	85ca                	mv	a1,s2
  a295a6:	08040513          	addi	a0,s0,128
  a295aa:	339d                	jal	ra,a29310 <secure_authenticate.constprop.2>
  a295ac:	c911                	beqz	a0,a295c0 <verify_image_head+0x190>
  a295ae:	00a3 2574 051f      	l.li	a0,0xa32574
  a295b4:	526030ef          	jal	ra,a2cada <boot_msg0>
  a295b8:	8000 15c3 041f      	l.li	s0,0x800015c3
  a295be:	bf55                	j	a29572 <verify_image_head+0x142>
  a295c0:	c08d                	beqz	s1,a295e2 <verify_image_head+0x1b2>
  a295c2:	01492603          	lw	a2,20(s2)
  a295c6:	01092583          	lw	a1,16(s2)
  a295ca:	8526                	mv	a0,s1
  a295cc:	33fd                	jal	ra,a293ba <check_version>
  a295ce:	c911                	beqz	a0,a295e2 <verify_image_head+0x1b2>
  a295d0:	00a3 25a0 051f      	l.li	a0,0xa325a0
  a295d6:	504030ef          	jal	ra,a2cada <boot_msg0>
  a295da:	8000 15c4 041f      	l.li	s0,0x800015c4
  a295e0:	bf49                	j	a29572 <verify_image_head+0x142>
  a295e2:	01c92583          	lw	a1,28(s2)
  a295e6:	01892503          	lw	a0,24(s2)
  a295ea:	39cd                	jal	ra,a292dc <check_msid>
  a295ec:	842a                	mv	s0,a0
  a295ee:	e80502e3          	beqz	a0,a29472 <verify_image_head+0x42>
  a295f2:	00a3 25cc 051f      	l.li	a0,0xa325cc
  a295f8:	4e2030ef          	jal	ra,a2cada <boot_msg0>
  a295fc:	8000 15c5 041f      	l.li	s0,0x800015c5
  a29602:	bf85                	j	a29572 <verify_image_head+0x142>
  a29604:	8000 15ca 041f      	l.li	s0,0x800015ca
  a2960a:	b5a5                	j	a29472 <verify_image_head+0x42>

00a2960c <verify_image_body>:
  a2960c:	8248                	push	{ra,s0-s2},-48
  a2960e:	f0000793          	li	a5,-256
  a29612:	06f50763          	beq	a0,a5,a29680 <verify_image_body+0x74>
  a29616:	c5ad                	beqz	a1,a29680 <verify_image_body+0x74>
  a29618:	84ae                	mv	s1,a1
  a2961a:	842a                	mv	s0,a0
  a2961c:	02000613          	li	a2,32
  a29620:	4581                	li	a1,0
  a29622:	10050913          	addi	s2,a0,256
  a29626:	850a                	mv	a0,sp
  a29628:	b7cff0ef          	jal	ra,a289a4 <memset>
  a2962c:	12442583          	lw	a1,292(s0)
  a29630:	02000693          	li	a3,32
  a29634:	860a                	mv	a2,sp
  a29636:	8526                	mv	a0,s1
  a29638:	31b000ef          	jal	ra,a2a152 <drv_rom_cipher_sha256>
  a2963c:	c105                	beqz	a0,a2965c <verify_image_body+0x50>
  a2963e:	00a3 2304 051f      	l.li	a0,0xa32304
  a29644:	496030ef          	jal	ra,a2cada <boot_msg0>
  a29648:	8000 15c7 041f      	l.li	s0,0x800015c7
  a2964e:	00a3 2360 051f      	l.li	a0,0xa32360
  a29654:	486030ef          	jal	ra,a2cada <boot_msg0>
  a29658:	8522                	mv	a0,s0
  a2965a:	8244                	popret	{ra,s0-s2},48
  a2965c:	12840593          	addi	a1,s0,296
  a29660:	02000613          	li	a2,32
  a29664:	850a                	mv	a0,sp
  a29666:	e88ff0ef          	jal	ra,a28cee <memcmp>
  a2966a:	4401                	li	s0,0
  a2966c:	d575                	beqz	a0,a29658 <verify_image_body+0x4c>
  a2966e:	00a3 2334 051f      	l.li	a0,0xa32334
  a29674:	466030ef          	jal	ra,a2cada <boot_msg0>
  a29678:	8000 15c8 041f      	l.li	s0,0x800015c8
  a2967e:	bfc1                	j	a2964e <verify_image_body+0x42>
  a29680:	8000 15ca 041f      	l.li	s0,0x800015ca
  a29686:	bfc9                	j	a29658 <verify_image_body+0x4c>

00a29688 <non_os_enter_critical>:
  a29688:	8038                	push	{ra,s0-s1},-16
  a2968a:	300024f3          	csrr	s1,mstatus
  a2968e:	300477f3          	csrrci	a5,mstatus,8
  a29692:	00a05437          	lui	s0,0xa05
  a29696:	52440793          	addi	a5,s0,1316 # a05524 <g_non_os_critical_nesting>
  a2969a:	239e                	lhu	a5,0(a5)
  a2969c:	52440413          	addi	s0,s0,1316
  a296a0:	9fa1                	uxth	a5
  a296a2:	eb89                	bnez	a5,a296b4 <non_os_enter_critical+0x2c>
  a296a4:	00a0 551c 079f      	l.li	a5,0xa0551c
  a296aa:	439c                	lw	a5,0(a5)
  a296ac:	c781                	beqz	a5,a296b4 <non_os_enter_critical+0x2c>
  a296ae:	8586                	mv	a1,ra
  a296b0:	4501                	li	a0,0
  a296b2:	9782                	jalr	a5
  a296b4:	00a0 5518 079f      	l.li	a5,0xa05518
  a296ba:	439c                	lw	a5,0(a5)
  a296bc:	c789                	beqz	a5,a296c6 <non_os_enter_critical+0x3e>
  a296be:	8586                	mv	a1,ra
  a296c0:	2012                	lhu	a2,0(s0)
  a296c2:	4501                	li	a0,0
  a296c4:	9782                	jalr	a5
  a296c6:	201e                	lhu	a5,0(s0)
  a296c8:	9fa1                	uxth	a5
  a296ca:	eb89                	bnez	a5,a296dc <non_os_enter_critical+0x54>
  a296cc:	808d                	srli	s1,s1,0x3
  a296ce:	0014c493          	xori	s1,s1,1
  a296d2:	8885                	andi	s1,s1,1
  a296d4:	00a0 5520 079f      	l.li	a5,0xa05520
  a296da:	c384                	sw	s1,0(a5)
  a296dc:	201e                	lhu	a5,0(s0)
  a296de:	0785                	addi	a5,a5,1
  a296e0:	9fa1                	uxth	a5
  a296e2:	a01e                	sh	a5,0(s0)
  a296e4:	201e                	lhu	a5,0(s0)
  a296e6:	9fa1                	uxth	a5
  a296e8:	eb81                	bnez	a5,a296f8 <non_os_enter_critical+0x70>
  a296ea:	06300593          	li	a1,99
  a296ee:	03100513          	li	a0,49
  a296f2:	8030                	pop	{ra,s0-s1},16
  a296f4:	32a0406f          	j	a2da1e <panic>
  a296f8:	0ff0000f          	fence
  a296fc:	0ff0000f          	fence
  a29700:	8034                	popret	{ra,s0-s1},16

00a29702 <non_os_exit_critical>:
  a29702:	8038                	push	{ra,s0-s1},-16
  a29704:	00a05437          	lui	s0,0xa05
  a29708:	52440793          	addi	a5,s0,1316 # a05524 <g_non_os_critical_nesting>
  a2970c:	239e                	lhu	a5,0(a5)
  a2970e:	9fa1                	uxth	a5
  a29710:	eb81                	bnez	a5,a29720 <non_os_exit_critical+0x1e>
  a29712:	06f00593          	li	a1,111
  a29716:	03100513          	li	a0,49
  a2971a:	8030                	pop	{ra,s0-s1},16
  a2971c:	3020406f          	j	a2da1e <panic>
  a29720:	52440413          	addi	s0,s0,1316
  a29724:	201e                	lhu	a5,0(s0)
  a29726:	8486                	mv	s1,ra
  a29728:	17fd                	addi	a5,a5,-1
  a2972a:	9fa1                	uxth	a5
  a2972c:	a01e                	sh	a5,0(s0)
  a2972e:	00a0 5518 079f      	l.li	a5,0xa05518
  a29734:	439c                	lw	a5,0(a5)
  a29736:	c789                	beqz	a5,a29740 <non_os_exit_critical+0x3e>
  a29738:	8586                	mv	a1,ra
  a2973a:	2012                	lhu	a2,0(s0)
  a2973c:	4505                	li	a0,1
  a2973e:	9782                	jalr	a5
  a29740:	201e                	lhu	a5,0(s0)
  a29742:	9fa1                	uxth	a5
  a29744:	e785                	bnez	a5,a2976c <non_os_exit_critical+0x6a>
  a29746:	00a0 551c 079f      	l.li	a5,0xa0551c
  a2974c:	439c                	lw	a5,0(a5)
  a2974e:	c781                	beqz	a5,a29756 <non_os_exit_critical+0x54>
  a29750:	85a6                	mv	a1,s1
  a29752:	4505                	li	a0,1
  a29754:	9782                	jalr	a5
  a29756:	00a0 5520 079f      	l.li	a5,0xa05520
  a2975c:	439c                	lw	a5,0(a5)
  a2975e:	e399                	bnez	a5,a29764 <non_os_exit_critical+0x62>
  a29760:	300467f3          	csrrsi	a5,mstatus,8
  a29764:	0ff0000f          	fence
  a29768:	0ff0000f          	fence
  a2976c:	8034                	popret	{ra,s0-s1},16

00a2976e <malloc_register_funcs>:
  a2976e:	8018                	push	{ra},-16
  a29770:	862a                	mv	a2,a0
  a29772:	46b1                	li	a3,12
  a29774:	45b1                	li	a1,12
  a29776:	00a0 5528 051f      	l.li	a0,0xa05528
  a2977c:	1f4070ef          	jal	ra,a30970 <memcpy_s>
  a29780:	4501                	li	a0,0
  a29782:	8014                	popret	{ra},16

00a29784 <rom_malloc_init>:
  a29784:	8038                	push	{ra,s0-s1},-16
  a29786:	842e                	mv	s0,a1
  a29788:	46e1                	li	a3,24
  a2978a:	4601                	li	a2,0
  a2978c:	45e1                	li	a1,24
  a2978e:	84aa                	mv	s1,a0
  a29790:	25c070ef          	jal	ra,a309ec <memset_s>
  a29794:	ff840793          	addi	a5,s0,-8
  a29798:	00a0 5534 071f      	l.li	a4,0xa05534
  a2979e:	01b48513          	addi	a0,s1,27
  a297a2:	9bf1                	andi	a5,a5,-4
  a297a4:	9971                	andi	a0,a0,-4
  a297a6:	cb1c                	sw	a5,16(a4)
  a297a8:	17e1                	addi	a5,a5,-8
  a297aa:	8f89                	sub	a5,a5,a0
  a297ac:	c748                	sw	a0,12(a4)
  a297ae:	9fa1                	uxth	a5
  a297b0:	a15e                	sh	a5,4(a0)
  a297b2:	c71c                	sw	a5,8(a4)
  a297b4:	679d                	lui	a5,0x7
  a297b6:	07478793          	addi	a5,a5,116 # 7074 <ccause+0x60b2>
  a297ba:	a17e                	sh	a5,6(a0)
  a297bc:	00052023          	sw	zero,0(a0)
  a297c0:	8034                	popret	{ra,s0-s1},16

00a297c2 <malloc_init>:
  a297c2:	b7c9                	j	a29784 <rom_malloc_init>

00a297c4 <rom_malloc>:
  a297c4:	00a056b7          	lui	a3,0xa05
  a297c8:	53468793          	addi	a5,a3,1332 # a05534 <g_st_dfx>
  a297cc:	47dc                	lw	a5,12(a5)
  a297ce:	53468693          	addi	a3,a3,1332
  a297d2:	e399                	bnez	a5,a297d8 <rom_malloc+0x14>
  a297d4:	4501                	li	a0,0
  a297d6:	8082                	ret
  a297d8:	dd75                	beqz	a0,a297d4 <rom_malloc+0x10>
  a297da:	23f2                	lhu	a2,6(a5)
  a297dc:	671d                	lui	a4,0x7
  a297de:	07470813          	addi	a6,a4,116 # 7074 <ccause+0x60b2>
  a297e2:	01060663          	beq	a2,a6,a297ee <rom_malloc+0x2a>
  a297e6:	c6c70713          	addi	a4,a4,-916
  a297ea:	fee615e3          	bne	a2,a4,a297d4 <rom_malloc+0x10>
  a297ee:	050d                	addi	a0,a0,3
  a297f0:	9971                	andi	a0,a0,-4
  a297f2:	671d                	lui	a4,0x7
  a297f4:	9d21                	uxth	a0
  a297f6:	4881                	li	a7,0
  a297f8:	07470613          	addi	a2,a4,116 # 7074 <ccause+0x60b2>
  a297fc:	00850813          	addi	a6,a0,8
  a29800:	c6c70313          	addi	t1,a4,-916
  a29804:	0067de03          	lhu	t3,6(a5)
  a29808:	23da                	lhu	a4,4(a5)
  a2980a:	04ce1263          	bne	t3,a2,a2984e <rom_malloc+0x8a>
  a2980e:	02e87563          	bgeu	a6,a4,a29838 <rom_malloc+0x74>
  a29812:	8f09                	sub	a4,a4,a0
  a29814:	983e                	add	a6,a6,a5
  a29816:	1761                	addi	a4,a4,-8
  a29818:	00e81223          	sh	a4,4(a6)
  a2981c:	00c81323          	sh	a2,6(a6)
  a29820:	671d                	lui	a4,0x7
  a29822:	c6c70713          	addi	a4,a4,-916 # 6c6c <ccause+0x5caa>
  a29826:	c38c                	sw	a1,0(a5)
  a29828:	a3fa                	sh	a4,6(a5)
  a2982a:	4298                	lw	a4,0(a3)
  a2982c:	a3ca                	sh	a0,4(a5)
  a2982e:	953a                	add	a0,a0,a4
  a29830:	c288                	sw	a0,0(a3)
  a29832:	00878513          	addi	a0,a5,8
  a29836:	8082                	ret
  a29838:	00a76b63          	bltu	a4,a0,a2984e <rom_malloc+0x8a>
  a2983c:	661d                	lui	a2,0x7
  a2983e:	c6c60613          	addi	a2,a2,-916 # 6c6c <ccause+0x5caa>
  a29842:	c38c                	sw	a1,0(a5)
  a29844:	a3f2                	sh	a2,6(a5)
  a29846:	4290                	lw	a2,0(a3)
  a29848:	9732                	add	a4,a4,a2
  a2984a:	c298                	sw	a4,0(a3)
  a2984c:	b7dd                	j	a29832 <rom_malloc+0x6e>
  a2984e:	0721                	addi	a4,a4,8
  a29850:	97ba                	add	a5,a5,a4
  a29852:	4a98                	lw	a4,16(a3)
  a29854:	00e7ef63          	bltu	a5,a4,a29872 <rom_malloc+0xae>
  a29858:	f6089ee3          	bnez	a7,a297d4 <rom_malloc+0x10>
  a2985c:	46d8                	lw	a4,12(a3)
  a2985e:	235e                	lhu	a5,4(a4)
  a29860:	0106ae03          	lw	t3,16(a3)
  a29864:	07a1                	addi	a5,a5,8
  a29866:	00f708b3          	add	a7,a4,a5
  a2986a:	01c8ea63          	bltu	a7,t3,a2987e <rom_malloc+0xba>
  a2986e:	46dc                	lw	a5,12(a3)
  a29870:	4885                	li	a7,1
  a29872:	23fa                	lhu	a4,6(a5)
  a29874:	f86708e3          	beq	a4,t1,a29804 <rom_malloc+0x40>
  a29878:	f8c706e3          	beq	a4,a2,a29804 <rom_malloc+0x40>
  a2987c:	bfa1                	j	a297d4 <rom_malloc+0x10>
  a2987e:	00675e03          	lhu	t3,6(a4)
  a29882:	00ce1b63          	bne	t3,a2,a29898 <rom_malloc+0xd4>
  a29886:	0068de03          	lhu	t3,6(a7)
  a2988a:	00ce1763          	bne	t3,a2,a29898 <rom_malloc+0xd4>
  a2988e:	0048d883          	lhu	a7,4(a7)
  a29892:	97c6                	add	a5,a5,a7
  a29894:	a35e                	sh	a5,4(a4)
  a29896:	88ba                	mv	a7,a4
  a29898:	8746                	mv	a4,a7
  a2989a:	b7d1                	j	a2985e <rom_malloc+0x9a>

00a2989c <malloc>:
  a2989c:	8018                	push	{ra},-16
  a2989e:	8586                	mv	a1,ra
  a298a0:	8010                	pop	{ra},16
  a298a2:	b70d                	j	a297c4 <rom_malloc>

00a298a4 <rom_free>:
  a298a4:	c50d                	beqz	a0,a298ce <rom_free+0x2a>
  a298a6:	ffe55703          	lhu	a4,-2(a0)
  a298aa:	0000 6c6c 079f      	l.li	a5,0x6c6c
  a298b0:	00f71f63          	bne	a4,a5,a298ce <rom_free+0x2a>
  a298b4:	679d                	lui	a5,0x7
  a298b6:	07478793          	addi	a5,a5,116 # 7074 <ccause+0x60b2>
  a298ba:	fef51f23          	sh	a5,-2(a0)
  a298be:	00a0 5534 071f      	l.li	a4,0xa05534
  a298c4:	ffc55683          	lhu	a3,-4(a0)
  a298c8:	435c                	lw	a5,4(a4)
  a298ca:	97b6                	add	a5,a5,a3
  a298cc:	c35c                	sw	a5,4(a4)
  a298ce:	4501                	li	a0,0
  a298d0:	8082                	ret

00a298d2 <free>:
  a298d2:	bfc9                	j	a298a4 <rom_free>

00a298d4 <osal_irq_lock>:
  a298d4:	8018                	push	{ra},-16
  a298d6:	3b4d                	jal	ra,a29688 <non_os_enter_critical>
  a298d8:	4501                	li	a0,0
  a298da:	8014                	popret	{ra},16

00a298dc <osal_irq_restore>:
  a298dc:	b51d                	j	a29702 <non_os_exit_critical>

00a298de <uapi_pmp_config>:
  a298de:	8168                	push	{ra,s0-s4},-48
  a298e0:	842a                	mv	s0,a0
  a298e2:	84ae                	mv	s1,a1
  a298e4:	033020ef          	jal	ra,a2c116 <pmp_port_register_hal_funcs>
  a298e8:	21c010ef          	jal	ra,a2ab04 <hal_pmp_get_funcs>
  a298ec:	89aa                	mv	s3,a0
  a298ee:	4901                	li	s2,0
  a298f0:	4a05                	li	s4,1
  a298f2:	00991663          	bne	s2,s1,a298fe <uapi_pmp_config+0x20>
  a298f6:	0ff0000f          	fence
  a298fa:	4501                	li	a0,0
  a298fc:	8164                	popret	{ra,s0-s4},48
  a298fe:	4058                	lw	a4,4(s0)
  a29900:	3454                	lbu	a3,13(s0)
  a29902:	00042803          	lw	a6,0(s0)
  a29906:	00842883          	lw	a7,8(s0)
  a2990a:	245c                	lbu	a5,12(s0)
  a2990c:	246c                	lbu	a1,14(s0)
  a2990e:	3468                	lbu	a0,15(s0)
  a29910:	00275613          	srli	a2,a4,0x2
  a29914:	01468763          	beq	a3,s4,a29922 <uapi_pmp_config+0x44>
  a29918:	177d                	addi	a4,a4,-1
  a2991a:	4317071b          	addshf	a4,a4,a7,srl,1
  a2991e:	00275613          	srli	a2,a4,0x2
  a29922:	0077f713          	andi	a4,a5,7
  a29926:	00d14783          	lbu	a5,13(sp)
  a2992a:	8a8d                	andi	a3,a3,3
  a2992c:	00a10623          	sb	a0,12(sp)
  a29930:	0e07f793          	andi	a5,a5,224
  a29934:	8fd9                	or	a5,a5,a4
  a29936:	06d7a79b          	orshf	a5,a5,a3,sll,3
  a2993a:	07f7f793          	andi	a5,a5,127
  a2993e:	0eb7a79b          	orshf	a5,a5,a1,sll,7
  a29942:	c242                	sw	a6,4(sp)
  a29944:	c432                	sw	a2,8(sp)
  a29946:	00f106a3          	sb	a5,13(sp)
  a2994a:	0009a783          	lw	a5,0(s3)
  a2994e:	0048                	addi	a0,sp,4
  a29950:	0441                	addi	s0,s0,16
  a29952:	9782                	jalr	a5
  a29954:	f545                	bnez	a0,a298fc <uapi_pmp_config+0x1e>
  a29956:	0905                	addi	s2,s2,1
  a29958:	bf69                	j	a298f2 <uapi_pmp_config+0x14>

00a2995a <drv_rom_hash>:
  a2995a:	8e78                	push	{ra,s0-s5},-256
  a2995c:	892a                	mv	s2,a0
  a2995e:	8aae                	mv	s5,a1
  a29960:	84b2                	mv	s1,a2
  a29962:	4581                	li	a1,0
  a29964:	04000613          	li	a2,64
  a29968:	1008                	addi	a0,sp,32
  a2996a:	c63a                	sw	a4,12(sp)
  a2996c:	8a36                	mv	s4,a3
  a2996e:	843e                	mv	s0,a5
  a29970:	834ff0ef          	jal	ra,a289a4 <memset>
  a29974:	08000613          	li	a2,128
  a29978:	4581                	li	a1,0
  a2997a:	1088                	addi	a0,sp,96
  a2997c:	828ff0ef          	jal	ra,a289a4 <memset>
  a29980:	ce02                	sw	zero,28(sp)
  a29982:	4732                	lw	a4,12(sp)
  a29984:	000a9463          	bnez	s5,a2998c <drv_rom_hash+0x32>
  a29988:	557d                	li	a0,-1
  a2998a:	8e74                	popret	{ra,s0-s5},256
  a2998c:	fe0a0ee3          	beqz	s4,a29988 <drv_rom_hash+0x2e>
  a29990:	010007b7          	lui	a5,0x1000
  a29994:	fef4fae3          	bgeu	s1,a5,a29988 <drv_rom_hash+0x2e>
  a29998:	20f71c3b          	bnei	a4,32,a29988 <drv_rom_hash+0x2e>
  a2999c:	0216 9100 079f      	l.li	a5,0x2169100
  a299a2:	0cf90963          	beq	s2,a5,a29a74 <drv_rom_hash+0x11a>
  a299a6:	00a3 2610 061f      	l.li	a2,0xa32610
  a299ac:	85ca                	mv	a1,s2
  a299ae:	8522                	mv	a0,s0
  a299b0:	77a000ef          	jal	ra,a2a12a <hal_cipher_hash_config>
  a299b4:	f979                	bnez	a0,a2998a <drv_rom_hash+0x30>
  a299b6:	03f4f913          	andi	s2,s1,63
  a299ba:	412489b3          	sub	s3,s1,s2
  a299be:	0a099f63          	bnez	s3,a29a7c <drv_rom_hash+0x122>
  a299c2:	08000693          	li	a3,128
  a299c6:	4601                	li	a2,0
  a299c8:	08000593          	li	a1,128
  a299cc:	1088                	addi	a0,sp,96
  a299ce:	01e070ef          	jal	ra,a309ec <memset_s>
  a299d2:	86ca                	mv	a3,s2
  a299d4:	013a8633          	add	a2,s5,s3
  a299d8:	08000593          	li	a1,128
  a299dc:	1088                	addi	a0,sp,96
  a299de:	793060ef          	jal	ra,a30970 <memcpy_s>
  a299e2:	f15d                	bnez	a0,a29988 <drv_rom_hash+0x2e>
  a299e4:	119c                	addi	a5,sp,224
  a299e6:	97ca                	add	a5,a5,s2
  a299e8:	f8000713          	li	a4,-128
  a299ec:	f8e78023          	sb	a4,-128(a5) # ffff80 <_gp_+0x5cb7c4>
  a299f0:	04000993          	li	s3,64
  a299f4:	3809623b          	bltui	s2,56,a299fc <drv_rom_hash+0xa2>
  a299f8:	08000993          	li	s3,128
  a299fc:	00349713          	slli	a4,s1,0x3
  a29a00:	01b49613          	slli	a2,s1,0x1b
  a29a04:	0000 ff00 079f      	l.li	a5,0xff00
  a29a0a:	50e7c79b          	andshf	a5,a5,a4,srl,8
  a29a0e:	70e6261b          	orshf	a2,a2,a4,srl,24
  a29a12:	8e5d                	or	a2,a2,a5
  a29a14:	00ff07b7          	lui	a5,0xff0
  a29a18:	1697c49b          	andshf	s1,a5,s1,sll,11
  a29a1c:	8e45                	or	a2,a2,s1
  a29a1e:	ffc98513          	addi	a0,s3,-4
  a29a22:	109c                	addi	a5,sp,96
  a29a24:	ce32                	sw	a2,28(sp)
  a29a26:	4691                	li	a3,4
  a29a28:	0870                	addi	a2,sp,28
  a29a2a:	4591                	li	a1,4
  a29a2c:	953e                	add	a0,a0,a5
  a29a2e:	743060ef          	jal	ra,a30970 <memcpy_s>
  a29a32:	f939                	bnez	a0,a29988 <drv_rom_hash+0x2e>
  a29a34:	4695                	li	a3,5
  a29a36:	864e                	mv	a2,s3
  a29a38:	108c                	addi	a1,sp,96
  a29a3a:	8522                	mv	a0,s0
  a29a3c:	6f6000ef          	jal	ra,a2a132 <hal_cipher_hash_add_in_node>
  a29a40:	f529                	bnez	a0,a2998a <drv_rom_hash+0x30>
  a29a42:	4581                	li	a1,0
  a29a44:	8522                	mv	a0,s0
  a29a46:	6f4000ef          	jal	ra,a2a13a <hal_cipher_hash_start>
  a29a4a:	f121                	bnez	a0,a2998a <drv_rom_hash+0x30>
  a29a4c:	04000613          	li	a2,64
  a29a50:	100c                	addi	a1,sp,32
  a29a52:	8522                	mv	a0,s0
  a29a54:	6ee000ef          	jal	ra,a2a142 <hal_cipher_hash_wait_done>
  a29a58:	f90d                	bnez	a0,a2998a <drv_rom_hash+0x30>
  a29a5a:	02000693          	li	a3,32
  a29a5e:	1010                	addi	a2,sp,32
  a29a60:	02000593          	li	a1,32
  a29a64:	8552                	mv	a0,s4
  a29a66:	70b060ef          	jal	ra,a30970 <memcpy_s>
  a29a6a:	00a03533          	snez	a0,a0
  a29a6e:	40a00533          	neg	a0,a0
  a29a72:	bf21                	j	a2998a <drv_rom_hash+0x30>
  a29a74:	00a3 2630 061f      	l.li	a2,0xa32630
  a29a7a:	bf0d                	j	a299ac <drv_rom_hash+0x52>
  a29a7c:	4695                	li	a3,5
  a29a7e:	864e                	mv	a2,s3
  a29a80:	85d6                	mv	a1,s5
  a29a82:	8522                	mv	a0,s0
  a29a84:	257d                	jal	ra,a2a132 <hal_cipher_hash_add_in_node>
  a29a86:	f00512e3          	bnez	a0,a2998a <drv_rom_hash+0x30>
  a29a8a:	4581                	li	a1,0
  a29a8c:	8522                	mv	a0,s0
  a29a8e:	2575                	jal	ra,a2a13a <hal_cipher_hash_start>
  a29a90:	ee051de3          	bnez	a0,a2998a <drv_rom_hash+0x30>
  a29a94:	4601                	li	a2,0
  a29a96:	4581                	li	a1,0
  a29a98:	8522                	mv	a0,s0
  a29a9a:	2565                	jal	ra,a2a142 <hal_cipher_hash_wait_done>
  a29a9c:	f20503e3          	beqz	a0,a299c2 <drv_rom_hash+0x68>
  a29aa0:	b5ed                	j	a2998a <drv_rom_hash+0x30>

00a29aa2 <drv_rom_cipher_create_keyslot>:
  a29aa2:	8248                	push	{ra,s0-s2},-48
  a29aa4:	8432                	mv	s0,a2
  a29aa6:	84aa                	mv	s1,a0
  a29aa8:	4639                	li	a2,14
  a29aaa:	892e                	mv	s2,a1
  a29aac:	01210513          	addi	a0,sp,18
  a29ab0:	4581                	li	a1,0
  a29ab2:	ef3fe0ef          	jal	ra,a289a4 <memset>
  a29ab6:	02000793          	li	a5,32
  a29aba:	00f10823          	sb	a5,16(sp)
  a29abe:	4581                	li	a1,0
  a29ac0:	4785                	li	a5,1
  a29ac2:	0048                	addi	a0,sp,4
  a29ac4:	c622                	sw	s0,12(sp)
  a29ac6:	00f108a3          	sb	a5,17(sp)
  a29aca:	00f10923          	sb	a5,18(sp)
  a29ace:	00f10b23          	sb	a5,22(sp)
  a29ad2:	00f10c23          	sb	a5,24(sp)
  a29ad6:	24f9                	jal	ra,a29da4 <drv_keyslot_create>
  a29ad8:	842a                	mv	s0,a0
  a29ada:	ed0d                	bnez	a0,a29b14 <drv_rom_cipher_create_keyslot+0x72>
  a29adc:	0028                	addi	a0,sp,8
  a29ade:	2e91                	jal	ra,a29e32 <drv_klad_create>
  a29ae0:	842a                	mv	s0,a0
  a29ae2:	e91d                	bnez	a0,a29b18 <drv_rom_cipher_create_keyslot+0x76>
  a29ae4:	4612                	lw	a2,4(sp)
  a29ae6:	4522                	lw	a0,8(sp)
  a29ae8:	4581                	li	a1,0
  a29aea:	2e4d                	jal	ra,a29e9c <drv_klad_attach>
  a29aec:	842a                	mv	s0,a0
  a29aee:	e10d                	bnez	a0,a29b10 <drv_rom_cipher_create_keyslot+0x6e>
  a29af0:	4522                	lw	a0,8(sp)
  a29af2:	006c                	addi	a1,sp,12
  a29af4:	21a1                	jal	ra,a29f3c <drv_klad_set_attr>
  a29af6:	842a                	mv	s0,a0
  a29af8:	e901                	bnez	a0,a29b08 <drv_rom_cipher_create_keyslot+0x66>
  a29afa:	4522                	lw	a0,8(sp)
  a29afc:	85ca                	mv	a1,s2
  a29afe:	2155                	jal	ra,a29fa2 <drv_klad_set_effective_key>
  a29b00:	842a                	mv	s0,a0
  a29b02:	e119                	bnez	a0,a29b08 <drv_rom_cipher_create_keyslot+0x66>
  a29b04:	4792                	lw	a5,4(sp)
  a29b06:	c09c                	sw	a5,0(s1)
  a29b08:	4612                	lw	a2,4(sp)
  a29b0a:	4522                	lw	a0,8(sp)
  a29b0c:	4581                	li	a1,0
  a29b0e:	2ec5                	jal	ra,a29efe <drv_klad_detach>
  a29b10:	4522                	lw	a0,8(sp)
  a29b12:	2e99                	jal	ra,a29e68 <drv_klad_destroy>
  a29b14:	8522                	mv	a0,s0
  a29b16:	8244                	popret	{ra,s0-s2},48
  a29b18:	4088                	lw	a0,0(s1)
  a29b1a:	24c5                	jal	ra,a29dfa <drv_keyslot_destroy>
  a29b1c:	bfe5                	j	a29b14 <drv_rom_cipher_create_keyslot+0x72>

00a29b1e <drv_rom_cipher_destroy_keyslot>:
  a29b1e:	acf1                	j	a29dfa <drv_keyslot_destroy>

00a29b20 <drv_rom_cipher_config_odrk1>:
  a29b20:	8238                	push	{ra,s0-s1},-48
  a29b22:	4639                	li	a2,14
  a29b24:	4581                	li	a1,0
  a29b26:	84aa                	mv	s1,a0
  a29b28:	01210513          	addi	a0,sp,18
  a29b2c:	e79fe0ef          	jal	ra,a289a4 <memset>
  a29b30:	0300 000d 079f      	l.li	a5,0x300000d
  a29b36:	c63e                	sw	a5,12(sp)
  a29b38:	02000793          	li	a5,32
  a29b3c:	00f10823          	sb	a5,16(sp)
  a29b40:	0028                	addi	a0,sp,8
  a29b42:	4785                	li	a5,1
  a29b44:	00f108a3          	sb	a5,17(sp)
  a29b48:	00f10b23          	sb	a5,22(sp)
  a29b4c:	00f10c23          	sb	a5,24(sp)
  a29b50:	24cd                	jal	ra,a29e32 <drv_klad_create>
  a29b52:	842a                	mv	s0,a0
  a29b54:	e515                	bnez	a0,a29b80 <drv_rom_cipher_config_odrk1+0x60>
  a29b56:	4522                	lw	a0,8(sp)
  a29b58:	4601                	li	a2,0
  a29b5a:	4589                	li	a1,2
  a29b5c:	2681                	jal	ra,a29e9c <drv_klad_attach>
  a29b5e:	842a                	mv	s0,a0
  a29b60:	ed11                	bnez	a0,a29b7c <drv_rom_cipher_config_odrk1+0x5c>
  a29b62:	4522                	lw	a0,8(sp)
  a29b64:	006c                	addi	a1,sp,12
  a29b66:	2ed9                	jal	ra,a29f3c <drv_klad_set_attr>
  a29b68:	842a                	mv	s0,a0
  a29b6a:	e509                	bnez	a0,a29b74 <drv_rom_cipher_config_odrk1+0x54>
  a29b6c:	4522                	lw	a0,8(sp)
  a29b6e:	85a6                	mv	a1,s1
  a29b70:	290d                	jal	ra,a29fa2 <drv_klad_set_effective_key>
  a29b72:	842a                	mv	s0,a0
  a29b74:	4522                	lw	a0,8(sp)
  a29b76:	4601                	li	a2,0
  a29b78:	4589                	li	a1,2
  a29b7a:	2651                	jal	ra,a29efe <drv_klad_detach>
  a29b7c:	4522                	lw	a0,8(sp)
  a29b7e:	24ed                	jal	ra,a29e68 <drv_klad_destroy>
  a29b80:	8522                	mv	a0,s0
  a29b82:	8234                	popret	{ra,s0-s1},48

00a29b84 <drv_rom_cipher_fapc_config>:
  a29b84:	8348                	push	{ra,s0-s2},-64
  a29b86:	842e                	mv	s0,a1
  a29b88:	8932                	mv	s2,a2
  a29b8a:	84aa                	mv	s1,a0
  a29b8c:	4661                	li	a2,24
  a29b8e:	4581                	li	a1,0
  a29b90:	0828                	addi	a0,sp,24
  a29b92:	c636                	sw	a3,12(sp)
  a29b94:	c43a                	sw	a4,8(sp)
  a29b96:	e0ffe0ef          	jal	ra,a289a4 <memset>
  a29b9a:	4789                	li	a5,2
  a29b9c:	46b2                	lw	a3,12(sp)
  a29b9e:	d43e                	sw	a5,40(sp)
  a29ba0:	10100793          	li	a5,257
  a29ba4:	c822                	sw	s0,16(sp)
  a29ba6:	ca4a                	sw	s2,20(sp)
  a29ba8:	ce22                	sw	s0,28(sp)
  a29baa:	02f11623          	sh	a5,44(sp)
  a29bae:	557d                	li	a0,-1
  a29bb0:	ce81                	beqz	a3,a29bc8 <drv_rom_cipher_fapc_config+0x44>
  a29bb2:	4722                	lw	a4,8(sp)
  a29bb4:	1007153b          	bnei	a4,16,a29bc8 <drv_rom_cipher_fapc_config+0x44>
  a29bb8:	4641                	li	a2,16
  a29bba:	85b6                	mv	a1,a3
  a29bbc:	4501                	li	a0,0
  a29bbe:	2971                	jal	ra,a2a05a <drv_fapc_set_iv>
  a29bc0:	e501                	bnez	a0,a29bc8 <drv_rom_cipher_fapc_config+0x44>
  a29bc2:	080c                	addi	a1,sp,16
  a29bc4:	8526                	mv	a0,s1
  a29bc6:	2155                	jal	ra,a2a06a <drv_fapc_set_config>
  a29bc8:	8344                	popret	{ra,s0-s2},64

00a29bca <drv_rom_cipher_fapc_bypass_config>:
  a29bca:	a325                	j	a2a0f2 <drv_fapc_set_bypass_config>

00a29bcc <inner_symc_crypto>:
  a29bcc:	8168                	push	{ra,s0-s4},-48
  a29bce:	03014803          	lbu	a6,48(sp)
  a29bd2:	104618bb          	bnei	a2,16,a29cf4 <inner_symc_crypto+0x128>
  a29bd6:	01620737          	lui	a4,0x1620
  a29bda:	00080463          	beqz	a6,a29be2 <inner_symc_crypto+0x16>
  a29bde:	11620737          	lui	a4,0x11620
  a29be2:	1ff57513          	andi	a0,a0,511
  a29be6:	e0077713          	andi	a4,a4,-512
  a29bea:	8f49                	or	a4,a4,a0
  a29bec:	441044b7          	lui	s1,0x44104
  a29bf0:	8946                	mv	s2,a7
  a29bf2:	8a3e                	mv	s4,a5
  a29bf4:	842e                	mv	s0,a1
  a29bf6:	10e4a823          	sw	a4,272(s1) # 44104110 <_gp_+0x436cf954>
  a29bfa:	0ff0000f          	fence
  a29bfe:	1004a223          	sw	zero,260(s1)
  a29c02:	0ff0000f          	fence
  a29c06:	00a0 55ac 099f      	l.li	s3,0xa055ac
  a29c0c:	0009a703          	lw	a4,0(s3)
  a29c10:	00a0 554c 051f      	l.li	a0,0xa0554c
  a29c16:	0716                	slli	a4,a4,0x5
  a29c18:	00e50633          	add	a2,a0,a4
  a29c1c:	420c                	lw	a1,0(a2)
  a29c1e:	0741                	addi	a4,a4,16 # 11620010 <_gp_+0x10beb854>
  a29c20:	01162223          	sw	a7,4(a2)
  a29c24:	0035e593          	ori	a1,a1,3
  a29c28:	dff5f593          	andi	a1,a1,-513
  a29c2c:	c20c                	sw	a1,0(a2)
  a29c2e:	c614                	sw	a3,8(a2)
  a29c30:	00062623          	sw	zero,12(a2)
  a29c34:	46c1                	li	a3,16
  a29c36:	8622                	mv	a2,s0
  a29c38:	45c1                	li	a1,16
  a29c3a:	953a                	add	a0,a0,a4
  a29c3c:	535060ef          	jal	ra,a30970 <memcpy_s>
  a29c40:	0009a703          	lw	a4,0(s3)
  a29c44:	00a0 558c 069f      	l.li	a3,0xa0558c
  a29c4a:	08e6869b          	addshf	a3,a3,a4,sll,4
  a29c4e:	0705                	addi	a4,a4,1
  a29c50:	8b05                	andi	a4,a4,1
  a29c52:	00e9a023          	sw	a4,0(s3)
  a29c56:	0126a223          	sw	s2,4(a3)
  a29c5a:	0146a423          	sw	s4,8(a3)
  a29c5e:	0006a623          	sw	zero,12(a3)
  a29c62:	4410d6b7          	lui	a3,0x4410d
  a29c66:	5a9c                	lw	a5,48(a3)
  a29c68:	00178713          	addi	a4,a5,1 # ff0001 <_gp_+0x5bb845>
  a29c6c:	8b05                	andi	a4,a4,1
  a29c6e:	f007f793          	andi	a5,a5,-256
  a29c72:	8fd9                	or	a5,a5,a4
  a29c74:	da9c                	sw	a5,48(a3)
  a29c76:	0ff0000f          	fence
  a29c7a:	1304a783          	lw	a5,304(s1)
  a29c7e:	00178713          	addi	a4,a5,1
  a29c82:	8b05                	andi	a4,a4,1
  a29c84:	f007f793          	andi	a5,a5,-256
  a29c88:	8fd9                	or	a5,a5,a4
  a29c8a:	12f4a823          	sw	a5,304(s1)
  a29c8e:	0ff0000f          	fence
  a29c92:	0000 2710 049f      	l.li	s1,0x2710
  a29c98:	4410c937          	lui	s2,0x4410c
  a29c9c:	79c1                	lui	s3,0xffff0
  a29c9e:	00092783          	lw	a5,0(s2) # 4410c000 <_gp_+0x436d7844>
  a29ca2:	0027f713          	andi	a4,a5,2
  a29ca6:	0137f7b3          	and	a5,a5,s3
  a29caa:	8fd9                	or	a5,a5,a4
  a29cac:	cf0d                	beqz	a4,a29ce6 <inner_symc_crypto+0x11a>
  a29cae:	00f92023          	sw	a5,0(s2)
  a29cb2:	0ff0000f          	fence
  a29cb6:	441097b7          	lui	a5,0x44109
  a29cba:	0807a703          	lw	a4,128(a5) # 44109080 <_gp_+0x436d48c4>
  a29cbe:	46c1                	li	a3,16
  a29cc0:	860a                	mv	a2,sp
  a29cc2:	c03a                	sw	a4,0(sp)
  a29cc4:	0847a703          	lw	a4,132(a5)
  a29cc8:	45c1                	li	a1,16
  a29cca:	8522                	mv	a0,s0
  a29ccc:	c23a                	sw	a4,4(sp)
  a29cce:	0887a703          	lw	a4,136(a5)
  a29cd2:	c43a                	sw	a4,8(sp)
  a29cd4:	08c7a783          	lw	a5,140(a5)
  a29cd8:	c63e                	sw	a5,12(sp)
  a29cda:	497060ef          	jal	ra,a30970 <memcpy_s>
  a29cde:	cd01                	beqz	a0,a29cf6 <inner_symc_crypto+0x12a>
  a29ce0:	04000513          	li	a0,64
  a29ce4:	a809                	j	a29cf6 <inner_symc_crypto+0x12a>
  a29ce6:	4505                	li	a0,1
  a29ce8:	14fd                	addi	s1,s1,-1
  a29cea:	236d                	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a29cec:	f8cd                	bnez	s1,a29c9e <inner_symc_crypto+0xd2>
  a29cee:	0b400513          	li	a0,180
  a29cf2:	a011                	j	a29cf6 <inner_symc_crypto+0x12a>
  a29cf4:	557d                	li	a0,-1
  a29cf6:	8164                	popret	{ra,s0-s4},48

00a29cf8 <drv_rom_cipher_symc_init>:
  a29cf8:	441007b7          	lui	a5,0x44100
  a29cfc:	4741                	li	a4,16
  a29cfe:	d398                	sw	a4,32(a5)
  a29d00:	0ff0000f          	fence
  a29d04:	441047b7          	lui	a5,0x44104
  a29d08:	00a0 554c 071f      	l.li	a4,0xa0554c
  a29d0e:	12e7a223          	sw	a4,292(a5) # 44104124 <_gp_+0x436cf968>
  a29d12:	0ff0000f          	fence
  a29d16:	4689                	li	a3,2
  a29d18:	12d7a623          	sw	a3,300(a5)
  a29d1c:	0ff0000f          	fence
  a29d20:	4410d737          	lui	a4,0x4410d
  a29d24:	00a0 558c 061f      	l.li	a2,0xa0558c
  a29d2a:	d350                	sw	a2,36(a4)
  a29d2c:	0ff0000f          	fence
  a29d30:	d714                	sw	a3,40(a4)
  a29d32:	0ff0000f          	fence
  a29d36:	8000 aa00 071f      	l.li	a4,0x8000aa00
  a29d3c:	10e7a023          	sw	a4,256(a5)
  a29d40:	0ff0000f          	fence
  a29d44:	4501                	li	a0,0
  a29d46:	8082                	ret

00a29d48 <drv_rom_cipher_symc_deinit>:
  a29d48:	441007b7          	lui	a5,0x44100
  a29d4c:	0207a023          	sw	zero,32(a5) # 44100020 <_gp_+0x436cb864>
  a29d50:	0ff0000f          	fence
  a29d54:	4501                	li	a0,0
  a29d56:	8082                	ret

00a29d58 <drv_rom_cipher_symc_decrypt>:
  a29d58:	8118                	push	{ra},-32
  a29d5a:	4305                	li	t1,1
  a29d5c:	c01a                	sw	t1,0(sp)
  a29d5e:	35bd                	jal	ra,a29bcc <inner_symc_crypto>
  a29d60:	8114                	popret	{ra},32

00a29d62 <crypto_get_cpu_type>:
  a29d62:	00a0 55b0 079f      	l.li	a5,0xa055b0
  a29d68:	01c7a303          	lw	t1,28(a5)
  a29d6c:	00030363          	beqz	t1,a29d72 <crypto_get_cpu_type+0x10>
  a29d70:	8302                	jr	t1
  a29d72:	4505                	li	a0,1
  a29d74:	8082                	ret

00a29d76 <crypto_sm_support>:
  a29d76:	4505                	li	a0,1
  a29d78:	8082                	ret

00a29d7a <crypto_get_value_by_index>:
  a29d7a:	57fd                	li	a5,-1
  a29d7c:	c105                	beqz	a0,a29d9c <crypto_get_value_by_index+0x22>
  a29d7e:	ce99                	beqz	a3,a29d9c <crypto_get_value_by_index+0x22>
  a29d80:	4781                	li	a5,0
  a29d82:	00b79463          	bne	a5,a1,a29d8a <crypto_get_value_by_index+0x10>
  a29d86:	57fd                	li	a5,-1
  a29d88:	a811                	j	a29d9c <crypto_get_value_by_index+0x22>
  a29d8a:	872a                	mv	a4,a0
  a29d8c:	0521                	addi	a0,a0,8
  a29d8e:	ff852803          	lw	a6,-8(a0)
  a29d92:	00c81763          	bne	a6,a2,a29da0 <crypto_get_value_by_index+0x26>
  a29d96:	435c                	lw	a5,4(a4)
  a29d98:	c29c                	sw	a5,0(a3)
  a29d9a:	4781                	li	a5,0
  a29d9c:	853e                	mv	a0,a5
  a29d9e:	8082                	ret
  a29da0:	0785                	addi	a5,a5,1
  a29da2:	b7c5                	j	a29d82 <crypto_get_value_by_index+0x8>

00a29da4 <drv_keyslot_create>:
  a29da4:	8158                	push	{ra,s0-s3},-48
  a29da6:	57fd                	li	a5,-1
  a29da8:	c63e                	sw	a5,12(sp)
  a29daa:	e119                	bnez	a0,a29db0 <drv_keyslot_create+0xc>
  a29dac:	557d                	li	a0,-1
  a29dae:	8154                	popret	{ra,s0-s3},48
  a29db0:	0205933b          	bnei	a1,2,a29dbc <drv_keyslot_create+0x18>
  a29db4:	00052023          	sw	zero,0(a0)
  a29db8:	4501                	li	a0,0
  a29dba:	bfd5                	j	a29dae <drv_keyslot_create+0xa>
  a29dbc:	c195                	beqz	a1,a29de0 <drv_keyslot_create+0x3c>
  a29dbe:	01f59bbb          	bnei	a1,1,a29dac <drv_keyslot_create+0x8>
  a29dc2:	4989                	li	s3,2
  a29dc4:	842e                	mv	s0,a1
  a29dc6:	892a                	mv	s2,a0
  a29dc8:	4481                	li	s1,0
  a29dca:	85a2                	mv	a1,s0
  a29dcc:	8526                	mv	a0,s1
  a29dce:	110010ef          	jal	ra,a2aede <hal_keyslot_lock>
  a29dd2:	c62a                	sw	a0,12(sp)
  a29dd4:	47b2                	lw	a5,12(sp)
  a29dd6:	c799                	beqz	a5,a29de4 <drv_keyslot_create+0x40>
  a29dd8:	0485                	addi	s1,s1,1
  a29dda:	fe9998e3          	bne	s3,s1,a29dca <drv_keyslot_create+0x26>
  a29dde:	b7f9                	j	a29dac <drv_keyslot_create+0x8>
  a29de0:	49a1                	li	s3,8
  a29de2:	b7cd                	j	a29dc4 <drv_keyslot_create+0x20>
  a29de4:	fd34f4e3          	bgeu	s1,s3,a29dac <drv_keyslot_create+0x8>
  a29de8:	550005b7          	lui	a1,0x55000
  a29dec:	2085a59b          	orshf	a1,a1,s0,sll,16
  a29df0:	8ccd                	or	s1,s1,a1
  a29df2:	00992023          	sw	s1,0(s2)
  a29df6:	4532                	lw	a0,12(sp)
  a29df8:	bf5d                	j	a29dae <drv_keyslot_create+0xa>

00a29dfa <drv_keyslot_destroy>:
  a29dfa:	c515                	beqz	a0,a29e26 <drv_keyslot_destroy+0x2c>
  a29dfc:	01855793          	srli	a5,a0,0x18
  a29e00:	550797bb          	bnei	a5,85,a29e1e <drv_keyslot_destroy+0x24>
  a29e04:	01055793          	srli	a5,a0,0x10
  a29e08:	0ff7f793          	andi	a5,a5,255
  a29e0c:	4705                	li	a4,1
  a29e0e:	00f76863          	bltu	a4,a5,a29e1e <drv_keyslot_destroy+0x24>
  a29e12:	01051693          	slli	a3,a0,0x10
  a29e16:	82c1                	srli	a3,a3,0x10
  a29e18:	e789                	bnez	a5,a29e22 <drv_keyslot_destroy+0x28>
  a29e1a:	0806e33b          	bltui	a3,8,a29e26 <drv_keyslot_destroy+0x2c>
  a29e1e:	557d                	li	a0,-1
  a29e20:	8082                	ret
  a29e22:	fed76ee3          	bltu	a4,a3,a29e1e <drv_keyslot_destroy+0x24>
  a29e26:	01055593          	srli	a1,a0,0x10
  a29e2a:	9d81                	uxtb	a1
  a29e2c:	9d21                	uxth	a0
  a29e2e:	14c0106f          	j	a2af7a <hal_keyslot_unlock>

00a29e32 <drv_klad_create>:
  a29e32:	e119                	bnez	a0,a29e38 <drv_klad_create+0x6>
  a29e34:	557d                	li	a0,-1
  a29e36:	8082                	ret
  a29e38:	8038                	push	{ra,s0-s1},-16
  a29e3a:	842a                	mv	s0,a0
  a29e3c:	00a05537          	lui	a0,0xa05
  a29e40:	5d450493          	addi	s1,a0,1492 # a055d4 <g_klad_ctx>
  a29e44:	02400693          	li	a3,36
  a29e48:	4601                	li	a2,0
  a29e4a:	02400593          	li	a1,36
  a29e4e:	5d450513          	addi	a0,a0,1492
  a29e52:	39b060ef          	jal	ra,a309ec <memset_s>
  a29e56:	4785                	li	a5,1
  a29e58:	02f48023          	sb	a5,32(s1)
  a29e5c:	2d3c 4b5a 079f      	l.li	a5,0x2d3c4b5a
  a29e62:	c01c                	sw	a5,0(s0)
  a29e64:	4501                	li	a0,0
  a29e66:	8034                	popret	{ra,s0-s1},16

00a29e68 <drv_klad_destroy>:
  a29e68:	8028                	push	{ra,s0},-16
  a29e6a:	2d3c 4b5a 079f      	l.li	a5,0x2d3c4b5a
  a29e70:	547d                	li	s0,-1
  a29e72:	02f51363          	bne	a0,a5,a29e98 <drv_klad_destroy+0x30>
  a29e76:	00a05537          	lui	a0,0xa05
  a29e7a:	5d450793          	addi	a5,a0,1492 # a055d4 <g_klad_ctx>
  a29e7e:	0207c783          	lbu	a5,32(a5)
  a29e82:	4401                	li	s0,0
  a29e84:	cb91                	beqz	a5,a29e98 <drv_klad_destroy+0x30>
  a29e86:	02400693          	li	a3,36
  a29e8a:	4601                	li	a2,0
  a29e8c:	02400593          	li	a1,36
  a29e90:	5d450513          	addi	a0,a0,1492
  a29e94:	359060ef          	jal	ra,a309ec <memset_s>
  a29e98:	8522                	mv	a0,s0
  a29e9a:	8024                	popret	{ra,s0},16

00a29e9c <drv_klad_attach>:
  a29e9c:	8148                	push	{ra,s0-s2},-32
  a29e9e:	57fd                	li	a5,-1
  a29ea0:	c63e                	sw	a5,12(sp)
  a29ea2:	2d3c 4b5a 079f      	l.li	a5,0x2d3c4b5a
  a29ea8:	04f51963          	bne	a0,a5,a29efa <drv_klad_attach+0x5e>
  a29eac:	00a05437          	lui	s0,0xa05
  a29eb0:	5d440793          	addi	a5,s0,1492 # a055d4 <g_klad_ctx>
  a29eb4:	0207c783          	lbu	a5,32(a5)
  a29eb8:	5d440413          	addi	s0,s0,1492
  a29ebc:	557d                	li	a0,-1
  a29ebe:	c385                	beqz	a5,a29ede <drv_klad_attach+0x42>
  a29ec0:	84ae                	mv	s1,a1
  a29ec2:	8932                	mv	s2,a2
  a29ec4:	0ff67593          	andi	a1,a2,255
  a29ec8:	0204863b          	beqi	s1,2,a29ee0 <drv_klad_attach+0x44>
  a29ecc:	85b2                	mv	a1,a2
  a29ece:	8526                	mv	a0,s1
  a29ed0:	358010ef          	jal	ra,a2b228 <hal_klad_set_key_addr>
  a29ed4:	c62a                	sw	a0,12(sp)
  a29ed6:	47b2                	lw	a5,12(sp)
  a29ed8:	458d                	li	a1,3
  a29eda:	c399                	beqz	a5,a29ee0 <drv_klad_attach+0x44>
  a29edc:	4532                	lw	a0,12(sp)
  a29ede:	8144                	popret	{ra,s0-s2},32
  a29ee0:	8526                	mv	a0,s1
  a29ee2:	2bc010ef          	jal	ra,a2b19e <hal_klad_set_key_dest_cfg>
  a29ee6:	c62a                	sw	a0,12(sp)
  a29ee8:	47b2                	lw	a5,12(sp)
  a29eea:	fbed                	bnez	a5,a29edc <drv_klad_attach+0x40>
  a29eec:	4785                	li	a5,1
  a29eee:	01242e23          	sw	s2,28(s0)
  a29ef2:	a044                	sb	s1,4(s0)
  a29ef4:	02f400a3          	sb	a5,33(s0)
  a29ef8:	b7d5                	j	a29edc <drv_klad_attach+0x40>
  a29efa:	557d                	li	a0,-1
  a29efc:	b7cd                	j	a29ede <drv_klad_attach+0x42>

00a29efe <drv_klad_detach>:
  a29efe:	2d3c 4b5a 079f      	l.li	a5,0x2d3c4b5a
  a29f04:	02f51a63          	bne	a0,a5,a29f38 <drv_klad_detach+0x3a>
  a29f08:	00a0 55d4 079f      	l.li	a5,0xa055d4
  a29f0e:	0207c703          	lbu	a4,32(a5)
  a29f12:	557d                	li	a0,-1
  a29f14:	c31d                	beqz	a4,a29f3a <drv_klad_detach+0x3c>
  a29f16:	23d8                	lbu	a4,4(a5)
  a29f18:	02b71163          	bne	a4,a1,a29f3a <drv_klad_detach+0x3c>
  a29f1c:	4fd8                	lw	a4,28(a5)
  a29f1e:	00c71e63          	bne	a4,a2,a29f3a <drv_klad_detach+0x3c>
  a29f22:	0217c703          	lbu	a4,33(a5)
  a29f26:	4501                	li	a0,0
  a29f28:	cb09                	beqz	a4,a29f3a <drv_klad_detach+0x3c>
  a29f2a:	020780a3          	sb	zero,33(a5)
  a29f2e:	0007ae23          	sw	zero,28(a5)
  a29f32:	00078223          	sb	zero,4(a5)
  a29f36:	8082                	ret
  a29f38:	557d                	li	a0,-1
  a29f3a:	8082                	ret

00a29f3c <drv_klad_set_attr>:
  a29f3c:	8138                	push	{ra,s0-s1},-32
  a29f3e:	57fd                	li	a5,-1
  a29f40:	c63e                	sw	a5,12(sp)
  a29f42:	e199                	bnez	a1,a29f48 <drv_klad_set_attr+0xc>
  a29f44:	557d                	li	a0,-1
  a29f46:	8134                	popret	{ra,s0-s1},32
  a29f48:	2d3c 4b5a 079f      	l.li	a5,0x2d3c4b5a
  a29f4e:	fef51be3          	bne	a0,a5,a29f44 <drv_klad_set_attr+0x8>
  a29f52:	00a0 55d4 049f      	l.li	s1,0xa055d4
  a29f58:	0204c783          	lbu	a5,32(s1)
  a29f5c:	d7e5                	beqz	a5,a29f44 <drv_klad_set_attr+0x8>
  a29f5e:	862e                	mv	a2,a1
  a29f60:	842e                	mv	s0,a1
  a29f62:	46d1                	li	a3,20
  a29f64:	45d1                	li	a1,20
  a29f66:	00848513          	addi	a0,s1,8
  a29f6a:	207060ef          	jal	ra,a30970 <memcpy_s>
  a29f6e:	c62a                	sw	a0,12(sp)
  a29f70:	47b2                	lw	a5,12(sp)
  a29f72:	fbe9                	bnez	a5,a29f44 <drv_klad_set_attr+0x8>
  a29f74:	2050                	lbu	a2,4(s0)
  a29f76:	304c                	lbu	a1,5(s0)
  a29f78:	2068                	lbu	a0,6(s0)
  a29f7a:	1ba010ef          	jal	ra,a2b134 <hal_klad_set_key_crypto_cfg>
  a29f7e:	c62a                	sw	a0,12(sp)
  a29f80:	47b2                	lw	a5,12(sp)
  a29f82:	c399                	beqz	a5,a29f88 <drv_klad_set_attr+0x4c>
  a29f84:	4532                	lw	a0,12(sp)
  a29f86:	b7c1                	j	a29f46 <drv_klad_set_attr+0xa>
  a29f88:	00740513          	addi	a0,s0,7
  a29f8c:	24a010ef          	jal	ra,a2b1d6 <hal_klad_set_key_secure_cfg>
  a29f90:	c62a                	sw	a0,12(sp)
  a29f92:	47b2                	lw	a5,12(sp)
  a29f94:	fbe5                	bnez	a5,a29f84 <drv_klad_set_attr+0x48>
  a29f96:	401c                	lw	a5,0(s0)
  a29f98:	c09c                	sw	a5,0(s1)
  a29f9a:	4785                	li	a5,1
  a29f9c:	02f48123          	sb	a5,34(s1)
  a29fa0:	b7d5                	j	a29f84 <drv_klad_set_attr+0x48>

00a29fa2 <drv_klad_set_effective_key>:
  a29fa2:	8238                	push	{ra,s0-s1},-48
  a29fa4:	57fd                	li	a5,-1
  a29fa6:	c43e                	sw	a5,8(sp)
  a29fa8:	c602                	sw	zero,12(sp)
  a29faa:	c802                	sw	zero,16(sp)
  a29fac:	ca02                	sw	zero,20(sp)
  a29fae:	cc02                	sw	zero,24(sp)
  a29fb0:	ce02                	sw	zero,28(sp)
  a29fb2:	e199                	bnez	a1,a29fb8 <drv_klad_set_effective_key+0x16>
  a29fb4:	557d                	li	a0,-1
  a29fb6:	a895                	j	a2a02a <drv_klad_set_effective_key+0x88>
  a29fb8:	41dc                	lw	a5,4(a1)
  a29fba:	dfed                	beqz	a5,a29fb4 <drv_klad_set_effective_key+0x12>
  a29fbc:	219c                	lbu	a5,0(a1)
  a29fbe:	842e                	mv	s0,a1
  a29fc0:	84aa                	mv	s1,a0
  a29fc2:	01078e3b          	beqi	a5,1,a29ffa <drv_klad_set_effective_key+0x58>
  a29fc6:	2d3c 4b5a 079f      	l.li	a5,0x2d3c4b5a
  a29fcc:	fef494e3          	bne	s1,a5,a29fb4 <drv_klad_set_effective_key+0x12>
  a29fd0:	00a054b7          	lui	s1,0xa05
  a29fd4:	5d448793          	addi	a5,s1,1492 # a055d4 <g_klad_ctx>
  a29fd8:	0207c703          	lbu	a4,32(a5)
  a29fdc:	5d448493          	addi	s1,s1,1492
  a29fe0:	db71                	beqz	a4,a29fb4 <drv_klad_set_effective_key+0x12>
  a29fe2:	0227c783          	lbu	a5,34(a5)
  a29fe6:	d7f9                	beqz	a5,a29fb4 <drv_klad_set_effective_key+0x12>
  a29fe8:	203c                	lbu	a5,2(s0)
  a29fea:	010782bb          	beqi	a5,1,a29ff4 <drv_klad_set_effective_key+0x52>
  a29fee:	cb91                	beqz	a5,a2a002 <drv_klad_set_effective_key+0x60>
  a29ff0:	02f7913b          	bnei	a5,2,a29fb4 <drv_klad_set_effective_key+0x12>
  a29ff4:	00f108a3          	sb	a5,17(sp)
  a29ff8:	a039                	j	a2a006 <drv_klad_set_effective_key+0x64>
  a29ffa:	4505                	li	a0,1
  a29ffc:	3bad                	jal	ra,a29d76 <crypto_sm_support>
  a29ffe:	f561                	bnez	a0,a29fc6 <drv_klad_set_effective_key+0x24>
  a2a000:	bf55                	j	a29fb4 <drv_klad_set_effective_key+0x12>
  a2a002:	000108a3          	sb	zero,17(sp)
  a2a006:	409c                	lw	a5,0(s1)
  a2a008:	c63e                	sw	a5,12(sp)
  a2a00a:	201c                	lbu	a5,0(s0)
  a2a00c:	00f10823          	sb	a5,16(sp)
  a2a010:	405c                	lw	a5,4(s0)
  a2a012:	ca3e                	sw	a5,20(sp)
  a2a014:	441c                	lw	a5,8(s0)
  a2a016:	cc3e                	sw	a5,24(sp)
  a2a018:	245c                	lbu	a5,12(s0)
  a2a01a:	00f10e23          	sb	a5,28(sp)
  a2a01e:	080010ef          	jal	ra,a2b09e <hal_klad_lock>
  a2a022:	c42a                	sw	a0,8(sp)
  a2a024:	47a2                	lw	a5,8(sp)
  a2a026:	c399                	beqz	a5,a2a02c <drv_klad_set_effective_key+0x8a>
  a2a028:	4522                	lw	a0,8(sp)
  a2a02a:	8234                	popret	{ra,s0-s1},48
  a2a02c:	4ab000ef          	jal	ra,a2acd6 <hal_rkp_lock>
  a2a030:	c42a                	sw	a0,8(sp)
  a2a032:	47a2                	lw	a5,8(sp)
  a2a034:	e385                	bnez	a5,a2a054 <drv_klad_set_effective_key+0xb2>
  a2a036:	0068                	addi	a0,sp,12
  a2a038:	531000ef          	jal	ra,a2ad68 <hal_rkp_kdf_hard_calculation>
  a2a03c:	c42a                	sw	a0,8(sp)
  a2a03e:	47a2                	lw	a5,8(sp)
  a2a040:	eb81                	bnez	a5,a2a050 <drv_klad_set_effective_key+0xae>
  a2a042:	20d0                	lbu	a2,4(s1)
  a2a044:	4532                	lw	a0,12(sp)
  a2a046:	85a2                	mv	a1,s0
  a2a048:	212010ef          	jal	ra,a2b25a <hal_klad_start_com_route>
  a2a04c:	c42a                	sw	a0,8(sp)
  a2a04e:	47a2                	lw	a5,8(sp)
  a2a050:	4c9000ef          	jal	ra,a2ad18 <hal_rkp_unlock>
  a2a054:	0a4010ef          	jal	ra,a2b0f8 <hal_klad_unlock>
  a2a058:	bfc1                	j	a2a028 <drv_klad_set_effective_key+0x86>

00a2a05a <drv_fapc_set_iv>:
  a2a05a:	8118                	push	{ra},-32
  a2a05c:	57fd                	li	a5,-1
  a2a05e:	c63e                	sw	a5,12(sp)
  a2a060:	34c010ef          	jal	ra,a2b3ac <hal_fapc_set_region_iv>
  a2a064:	c62a                	sw	a0,12(sp)
  a2a066:	4532                	lw	a0,12(sp)
  a2a068:	8114                	popret	{ra},32

00a2a06a <drv_fapc_set_config>:
  a2a06a:	8148                	push	{ra,s0-s2},-32
  a2a06c:	57fd                	li	a5,-1
  a2a06e:	c63e                	sw	a5,12(sp)
  a2a070:	0015b793          	seqz	a5,a1
  a2a074:	40f007b3          	neg	a5,a5
  a2a078:	c63e                	sw	a5,12(sp)
  a2a07a:	47b2                	lw	a5,12(sp)
  a2a07c:	c399                	beqz	a5,a2a082 <drv_fapc_set_config+0x18>
  a2a07e:	4532                	lw	a0,12(sp)
  a2a080:	8144                	popret	{ra,s0-s2},32
  a2a082:	4d84                	lw	s1,24(a1)
  a2a084:	41d0                	lw	a2,4(a1)
  a2a086:	842e                	mv	s0,a1
  a2a088:	418c                	lw	a1,0(a1)
  a2a08a:	892a                	mv	s2,a0
  a2a08c:	294010ef          	jal	ra,a2b320 <hal_fapc_set_region_addr>
  a2a090:	c62a                	sw	a0,12(sp)
  a2a092:	47b2                	lw	a5,12(sp)
  a2a094:	f7ed                	bnez	a5,a2a07e <drv_fapc_set_config+0x14>
  a2a096:	4850                	lw	a2,20(s0)
  a2a098:	480c                	lw	a1,16(s0)
  a2a09a:	854a                	mv	a0,s2
  a2a09c:	2a6010ef          	jal	ra,a2b342 <hal_fapc_set_region_permission>
  a2a0a0:	c62a                	sw	a0,12(sp)
  a2a0a2:	47b2                	lw	a5,12(sp)
  a2a0a4:	ffe9                	bnez	a5,a2a07e <drv_fapc_set_config+0x14>
  a2a0a6:	030494bb          	bnei	s1,3,a2a0b8 <drv_fapc_set_config+0x4e>
  a2a0aa:	440c                	lw	a1,8(s0)
  a2a0ac:	854a                	mv	a0,s2
  a2a0ae:	298010ef          	jal	ra,a2b346 <hal_fapc_set_region_mac_addr>
  a2a0b2:	c62a                	sw	a0,12(sp)
  a2a0b4:	47b2                	lw	a5,12(sp)
  a2a0b6:	f7e1                	bnez	a5,a2a07e <drv_fapc_set_config+0x14>
  a2a0b8:	85a6                	mv	a1,s1
  a2a0ba:	854a                	mv	a0,s2
  a2a0bc:	28e010ef          	jal	ra,a2b34a <hal_fapc_set_region_mode>
  a2a0c0:	c62a                	sw	a0,12(sp)
  a2a0c2:	47b2                	lw	a5,12(sp)
  a2a0c4:	ffcd                	bnez	a5,a2a07e <drv_fapc_set_config+0x14>
  a2a0c6:	2c4c                	lbu	a1,28(s0)
  a2a0c8:	854a                	mv	a0,s2
  a2a0ca:	2ba010ef          	jal	ra,a2b384 <hal_fapc_region_enable>
  a2a0ce:	c62a                	sw	a0,12(sp)
  a2a0d0:	47b2                	lw	a5,12(sp)
  a2a0d2:	f7d5                	bnez	a5,a2a07e <drv_fapc_set_config+0x14>
  a2a0d4:	444c                	lw	a1,12(s0)
  a2a0d6:	854a                	mv	a0,s2
  a2a0d8:	31c010ef          	jal	ra,a2b3f4 <hal_fapc_set_region_iv_start_addr>
  a2a0dc:	c62a                	sw	a0,12(sp)
  a2a0de:	47b2                	lw	a5,12(sp)
  a2a0e0:	ffd9                	bnez	a5,a2a07e <drv_fapc_set_config+0x14>
  a2a0e2:	3c5c                	lbu	a5,29(s0)
  a2a0e4:	dfc9                	beqz	a5,a2a07e <drv_fapc_set_config+0x14>
  a2a0e6:	854a                	mv	a0,s2
  a2a0e8:	2a0010ef          	jal	ra,a2b388 <hal_fapc_region_lock>
  a2a0ec:	c62a                	sw	a0,12(sp)
  a2a0ee:	47b2                	lw	a5,12(sp)
  a2a0f0:	b779                	j	a2a07e <drv_fapc_set_config+0x14>

00a2a0f2 <drv_fapc_set_bypass_config>:
  a2a0f2:	8228                	push	{ra,s0},-48
  a2a0f4:	57fd                	li	a5,-1
  a2a0f6:	c636                	sw	a3,12(sp)
  a2a0f8:	ce3e                	sw	a5,28(sp)
  a2a0fa:	842a                	mv	s0,a0
  a2a0fc:	224010ef          	jal	ra,a2b320 <hal_fapc_set_region_addr>
  a2a100:	ce2a                	sw	a0,28(sp)
  a2a102:	47f2                	lw	a5,28(sp)
  a2a104:	46b2                	lw	a3,12(sp)
  a2a106:	c399                	beqz	a5,a2a10c <drv_fapc_set_bypass_config+0x1a>
  a2a108:	4572                	lw	a0,28(sp)
  a2a10a:	8224                	popret	{ra,s0},48
  a2a10c:	def5                	beqz	a3,a2a108 <drv_fapc_set_bypass_config+0x16>
  a2a10e:	8522                	mv	a0,s0
  a2a110:	278010ef          	jal	ra,a2b388 <hal_fapc_region_lock>
  a2a114:	ce2a                	sw	a0,28(sp)
  a2a116:	47f2                	lw	a5,28(sp)
  a2a118:	bfc5                	j	a2a108 <drv_fapc_set_bypass_config+0x16>

00a2a11a <hal_hash_lock>:
  a2a11a:	0010 2fd4 031f      	l.li	t1,0x102fd4
  a2a120:	8302                	jr	t1

00a2a122 <hal_hash_unlock>:
  a2a122:	0010 2d8a 031f      	l.li	t1,0x102d8a
  a2a128:	8302                	jr	t1

00a2a12a <hal_cipher_hash_config>:
  a2a12a:	0010 2daa 031f      	l.li	t1,0x102daa
  a2a130:	8302                	jr	t1

00a2a132 <hal_cipher_hash_add_in_node>:
  a2a132:	0010 2eb4 031f      	l.li	t1,0x102eb4
  a2a138:	8302                	jr	t1

00a2a13a <hal_cipher_hash_start>:
  a2a13a:	0010 2f28 031f      	l.li	t1,0x102f28
  a2a140:	8302                	jr	t1

00a2a142 <hal_cipher_hash_wait_done>:
  a2a142:	0010 30fc 031f      	l.li	t1,0x1030fc
  a2a148:	8302                	jr	t1

00a2a14a <drv_rom_cipher_pke_bp256r_verify>:
  a2a14a:	0010 262a 031f      	l.li	t1,0x10262a
  a2a150:	8302                	jr	t1

00a2a152 <drv_rom_cipher_sha256>:
  a2a152:	8128                	push	{ra,s0},-32
  a2a154:	842a                	mv	s0,a0
  a2a156:	4505                	li	a0,1
  a2a158:	c62e                	sw	a1,12(sp)
  a2a15a:	c432                	sw	a2,8(sp)
  a2a15c:	c236                	sw	a3,4(sp)
  a2a15e:	3f75                	jal	ra,a2a11a <hal_hash_lock>
  a2a160:	4622                	lw	a2,8(sp)
  a2a162:	45b2                	lw	a1,12(sp)
  a2a164:	4712                	lw	a4,4(sp)
  a2a166:	86b2                	mv	a3,a2
  a2a168:	4785                	li	a5,1
  a2a16a:	862e                	mv	a2,a1
  a2a16c:	0116 9100 051f      	l.li	a0,0x1169100
  a2a172:	85a2                	mv	a1,s0
  a2a174:	fe6ff0ef          	jal	ra,a2995a <drv_rom_hash>
  a2a178:	842a                	mv	s0,a0
  a2a17a:	4505                	li	a0,1
  a2a17c:	375d                	jal	ra,a2a122 <hal_hash_unlock>
  a2a17e:	8522                	mv	a0,s0
  a2a180:	8124                	popret	{ra,s0},32

00a2a182 <uapi_systick_get_count>:
  a2a182:	8048                	push	{ra,s0-s2},-16
  a2a184:	f50ff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a188:	00a0 55f8 079f      	l.li	a5,0xa055f8
  a2a18e:	239c                	lbu	a5,0(a5)
  a2a190:	eb81                	bnez	a5,a2a1a0 <uapi_systick_get_count+0x1e>
  a2a192:	f4aff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a196:	4401                	li	s0,0
  a2a198:	4481                	li	s1,0
  a2a19a:	8522                	mv	a0,s0
  a2a19c:	85a6                	mv	a1,s1
  a2a19e:	8044                	popret	{ra,s0-s2},16
  a2a1a0:	892a                	mv	s2,a0
  a2a1a2:	70d010ef          	jal	ra,a2c0ae <hal_systick_get_count>
  a2a1a6:	842a                	mv	s0,a0
  a2a1a8:	854a                	mv	a0,s2
  a2a1aa:	84ae                	mv	s1,a1
  a2a1ac:	f30ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a1b0:	b7ed                	j	a2a19a <uapi_systick_get_count+0x18>

00a2a1b2 <uapi_systick_get_us>:
  a2a1b2:	8128                	push	{ra,s0},-32
  a2a1b4:	37f9                	jal	ra,a2a182 <uapi_systick_get_count>
  a2a1b6:	842a                	mv	s0,a0
  a2a1b8:	c62e                	sw	a1,12(sp)
  a2a1ba:	705010ef          	jal	ra,a2c0be <systick_clock_get>
  a2a1be:	45b2                	lw	a1,12(sp)
  a2a1c0:	000f 4240 079f      	l.li	a5,0xf4240
  a2a1c6:	02f43733          	mulhu	a4,s0,a5
  a2a1ca:	862a                	mv	a2,a0
  a2a1cc:	4681                	li	a3,0
  a2a1ce:	02f585b3          	mul	a1,a1,a5
  a2a1d2:	02f40533          	mul	a0,s0,a5
  a2a1d6:	95ba                	add	a1,a1,a4
  a2a1d8:	bc4fe0ef          	jal	ra,a2859c <__udivdi3>
  a2a1dc:	8124                	popret	{ra,s0},32

00a2a1de <uapi_tcxo_init>:
  a2a1de:	8048                	push	{ra,s0-s2},-16
  a2a1e0:	ef4ff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a1e4:	00a0 55f9 091f      	l.li	s2,0xa055f9
  a2a1ea:	00094783          	lbu	a5,0(s2)
  a2a1ee:	c791                	beqz	a5,a2a1fa <uapi_tcxo_init+0x1c>
  a2a1f0:	eecff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a1f4:	4401                	li	s0,0
  a2a1f6:	8522                	mv	a0,s0
  a2a1f8:	8044                	popret	{ra,s0-s2},16
  a2a1fa:	84aa                	mv	s1,a0
  a2a1fc:	214010ef          	jal	ra,a2b410 <hal_tcxo_init>
  a2a200:	842a                	mv	s0,a0
  a2a202:	c509                	beqz	a0,a2a20c <uapi_tcxo_init+0x2e>
  a2a204:	8526                	mv	a0,s1
  a2a206:	ed6ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a20a:	b7f5                	j	a2a1f6 <uapi_tcxo_init+0x18>
  a2a20c:	4785                	li	a5,1
  a2a20e:	00f90023          	sb	a5,0(s2)
  a2a212:	bfcd                	j	a2a204 <uapi_tcxo_init+0x26>

00a2a214 <uapi_tcxo_get_count>:
  a2a214:	8048                	push	{ra,s0-s2},-16
  a2a216:	ebeff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a21a:	00a0 55f9 079f      	l.li	a5,0xa055f9
  a2a220:	239c                	lbu	a5,0(a5)
  a2a222:	eb81                	bnez	a5,a2a232 <uapi_tcxo_get_count+0x1e>
  a2a224:	eb8ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a228:	4401                	li	s0,0
  a2a22a:	4481                	li	s1,0
  a2a22c:	8522                	mv	a0,s0
  a2a22e:	85a6                	mv	a1,s1
  a2a230:	8044                	popret	{ra,s0-s2},16
  a2a232:	892a                	mv	s2,a0
  a2a234:	208010ef          	jal	ra,a2b43c <hal_tcxo_get>
  a2a238:	842a                	mv	s0,a0
  a2a23a:	854a                	mv	a0,s2
  a2a23c:	84ae                	mv	s1,a1
  a2a23e:	e9eff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a242:	b7ed                	j	a2a22c <uapi_tcxo_get_count+0x18>

00a2a244 <uapi_tcxo_delay_ms>:
  a2a244:	00a0 55f9 079f      	l.li	a5,0xa055f9
  a2a24a:	239c                	lbu	a5,0(a5)
  a2a24c:	c3b1                	beqz	a5,a2a290 <uapi_tcxo_delay_ms+0x4c>
  a2a24e:	8048                	push	{ra,s0-s2},-16
  a2a250:	842a                	mv	s0,a0
  a2a252:	67f010ef          	jal	ra,a2c0d0 <tcxo_porting_ticks_per_usec_get>
  a2a256:	3e800793          	li	a5,1000
  a2a25a:	02f405b3          	mul	a1,s0,a5
  a2a25e:	02f43433          	mulhu	s0,s0,a5
  a2a262:	02a584b3          	mul	s1,a1,a0
  a2a266:	02a40433          	mul	s0,s0,a0
  a2a26a:	02a5b5b3          	mulhu	a1,a1,a0
  a2a26e:	942e                	add	s0,s0,a1
  a2a270:	3755                	jal	ra,a2a214 <uapi_tcxo_get_count>
  a2a272:	00a48933          	add	s2,s1,a0
  a2a276:	009934b3          	sltu	s1,s2,s1
  a2a27a:	95a2                	add	a1,a1,s0
  a2a27c:	94ae                	add	s1,s1,a1
  a2a27e:	3f59                	jal	ra,a2a214 <uapi_tcxo_get_count>
  a2a280:	fe95efe3          	bltu	a1,s1,a2a27e <uapi_tcxo_delay_ms+0x3a>
  a2a284:	00b49463          	bne	s1,a1,a2a28c <uapi_tcxo_delay_ms+0x48>
  a2a288:	ff256be3          	bltu	a0,s2,a2a27e <uapi_tcxo_delay_ms+0x3a>
  a2a28c:	4501                	li	a0,0
  a2a28e:	8044                	popret	{ra,s0-s2},16
  a2a290:	557d                	li	a0,-1
  a2a292:	8082                	ret

00a2a294 <uapi_tcxo_delay_us>:
  a2a294:	00a0 55f9 079f      	l.li	a5,0xa055f9
  a2a29a:	239c                	lbu	a5,0(a5)
  a2a29c:	cb8d                	beqz	a5,a2a2ce <uapi_tcxo_delay_us+0x3a>
  a2a29e:	8048                	push	{ra,s0-s2},-16
  a2a2a0:	842a                	mv	s0,a0
  a2a2a2:	62f010ef          	jal	ra,a2c0d0 <tcxo_porting_ticks_per_usec_get>
  a2a2a6:	02a404b3          	mul	s1,s0,a0
  a2a2aa:	02a43433          	mulhu	s0,s0,a0
  a2a2ae:	379d                	jal	ra,a2a214 <uapi_tcxo_get_count>
  a2a2b0:	00a48933          	add	s2,s1,a0
  a2a2b4:	009934b3          	sltu	s1,s2,s1
  a2a2b8:	942e                	add	s0,s0,a1
  a2a2ba:	9426                	add	s0,s0,s1
  a2a2bc:	3fa1                	jal	ra,a2a214 <uapi_tcxo_get_count>
  a2a2be:	fe85efe3          	bltu	a1,s0,a2a2bc <uapi_tcxo_delay_us+0x28>
  a2a2c2:	00b41463          	bne	s0,a1,a2a2ca <uapi_tcxo_delay_us+0x36>
  a2a2c6:	ff256be3          	bltu	a0,s2,a2a2bc <uapi_tcxo_delay_us+0x28>
  a2a2ca:	4501                	li	a0,0
  a2a2cc:	8044                	popret	{ra,s0-s2},16
  a2a2ce:	557d                	li	a0,-1
  a2a2d0:	8082                	ret

00a2a2d2 <uart_helper_invoke_current_fragment_callback>:
  a2a2d2:	00a0 5648 079f      	l.li	a5,0xa05648
  a2a2d8:	4ca7955b          	muliadd	a0,a5,a0,76
  a2a2dc:	411c                	lw	a5,0(a0)
  a2a2de:	0087a303          	lw	t1,8(a5)
  a2a2e2:	00030663          	beqz	t1,a2a2ee <uart_helper_invoke_current_fragment_callback+0x1c>
  a2a2e6:	43d0                	lw	a2,4(a5)
  a2a2e8:	47cc                	lw	a1,12(a5)
  a2a2ea:	4388                	lw	a0,0(a5)
  a2a2ec:	8302                	jr	t1
  a2a2ee:	8082                	ret

00a2a2f0 <uart_helper_move_to_next_fragment>:
  a2a2f0:	04c00793          	li	a5,76
  a2a2f4:	02f507b3          	mul	a5,a0,a5
  a2a2f8:	00a056b7          	lui	a3,0xa05
  a2a2fc:	64868713          	addi	a4,a3,1608 # a05648 <g_uart_tx_state_array>
  a2a300:	64868693          	addi	a3,a3,1608
  a2a304:	00f705b3          	add	a1,a4,a5
  a2a308:	4190                	lw	a2,0(a1)
  a2a30a:	07b1                	addi	a5,a5,12
  a2a30c:	97ba                	add	a5,a5,a4
  a2a30e:	0641                	addi	a2,a2,16
  a2a310:	04078713          	addi	a4,a5,64
  a2a314:	00e67b63          	bgeu	a2,a4,a2a32a <uart_helper_move_to_next_fragment+0x3a>
  a2a318:	c190                	sw	a2,0(a1)
  a2a31a:	4ca6955b          	muliadd	a0,a3,a0,76
  a2a31e:	251e                	lhu	a5,8(a0)
  a2a320:	00051523          	sh	zero,10(a0)
  a2a324:	17fd                	addi	a5,a5,-1
  a2a326:	a51e                	sh	a5,8(a0)
  a2a328:	8082                	ret
  a2a32a:	c19c                	sw	a5,0(a1)
  a2a32c:	b7fd                	j	a2a31a <uart_helper_move_to_next_fragment+0x2a>

00a2a32e <uart_helper_send_next_char>:
  a2a32e:	8038                	push	{ra,s0-s1},-16
  a2a330:	00a0 5648 041f      	l.li	s0,0xa05648
  a2a336:	4ca4145b          	muliadd	s0,s0,a0,76
  a2a33a:	4004                	lw	s1,0(s0)
  a2a33c:	243e                	lhu	a5,10(s0)
  a2a33e:	4605                	li	a2,1
  a2a340:	408c                	lw	a1,0(s1)
  a2a342:	95be                	add	a1,a1,a5
  a2a344:	1ae010ef          	jal	ra,a2b4f2 <hal_uart_write>
  a2a348:	242a                	lhu	a0,10(s0)
  a2a34a:	0505                	addi	a0,a0,1
  a2a34c:	9d21                	uxth	a0
  a2a34e:	a42a                	sh	a0,10(s0)
  a2a350:	44dc                	lw	a5,12(s1)
  a2a352:	00f53533          	sltu	a0,a0,a5
  a2a356:	00154513          	xori	a0,a0,1
  a2a35a:	8034                	popret	{ra,s0-s1},16

00a2a35c <uart_error_isr>:
  a2a35c:	8148                	push	{ra,s0-s2},-32
  a2a35e:	842a                	mv	s0,a0
  a2a360:	00f10613          	addi	a2,sp,15
  a2a364:	45a5                	li	a1,9
  a2a366:	00a0 5600 091f      	l.li	s2,0xa05600
  a2a36c:	00010723          	sb	zero,14(sp)
  a2a370:	000107a3          	sb	zero,15(sp)
  a2a374:	188914db          	muliadd	s1,s2,s0,24
  a2a378:	19e010ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a37c:	00f14783          	lbu	a5,15(sp)
  a2a380:	cf89                	beqz	a5,a2a39a <uart_error_isr+0x3e>
  a2a382:	1889145b          	muliadd	s0,s2,s0,24
  a2a386:	284e                	lhu	a1,20(s0)
  a2a388:	c981                	beqz	a1,a2a398 <uart_error_isr+0x3c>
  a2a38a:	401c                	lw	a5,0(s0)
  a2a38c:	c791                	beqz	a5,a2a398 <uart_error_isr+0x3c>
  a2a38e:	4448                	lw	a0,12(s0)
  a2a390:	4605                	li	a2,1
  a2a392:	9782                	jalr	a5
  a2a394:	00041a23          	sh	zero,20(s0)
  a2a398:	8144                	popret	{ra,s0-s2},32
  a2a39a:	00e10593          	addi	a1,sp,14
  a2a39e:	4605                	li	a2,1
  a2a3a0:	8522                	mv	a0,s0
  a2a3a2:	162010ef          	jal	ra,a2b504 <hal_uart_read>
  a2a3a6:	28d6                	lhu	a3,20(s1)
  a2a3a8:	44dc                	lw	a5,12(s1)
  a2a3aa:	00e14703          	lbu	a4,14(sp)
  a2a3ae:	97b6                	add	a5,a5,a3
  a2a3b0:	a398                	sb	a4,0(a5)
  a2a3b2:	28ce                	lhu	a1,20(s1)
  a2a3b4:	289e                	lhu	a5,16(s1)
  a2a3b6:	0585                	addi	a1,a1,1 # 55000001 <_gp_+0x545cb845>
  a2a3b8:	9da1                	uxth	a1
  a2a3ba:	a8ce                	sh	a1,20(s1)
  a2a3bc:	00f5e963          	bltu	a1,a5,a2a3ce <uart_error_isr+0x72>
  a2a3c0:	409c                	lw	a5,0(s1)
  a2a3c2:	c781                	beqz	a5,a2a3ca <uart_error_isr+0x6e>
  a2a3c4:	44c8                	lw	a0,12(s1)
  a2a3c6:	4605                	li	a2,1
  a2a3c8:	9782                	jalr	a5
  a2a3ca:	00049a23          	sh	zero,20(s1)
  a2a3ce:	00f10613          	addi	a2,sp,15
  a2a3d2:	45a5                	li	a1,9
  a2a3d4:	8522                	mv	a0,s0
  a2a3d6:	140010ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a3da:	b74d                	j	a2a37c <uart_error_isr+0x20>

00a2a3dc <uart_evt_callback>:
  a2a3dc:	8158                	push	{ra,s0-s3},-48
  a2a3de:	15fd                	addi	a1,a1,-1
  a2a3e0:	0ff5f793          	andi	a5,a1,255
  a2a3e4:	0567fabb          	bgeui	a5,5,a2a58e <uart_evt_callback+0x1b2>
  a2a3e8:	00a325b7          	lui	a1,0xa32
  a2a3ec:	65058593          	addi	a1,a1,1616 # a32650 <g_sm3_ival+0x20>
  a2a3f0:	04f5859b          	addshf	a1,a1,a5,sll,2
  a2a3f4:	419c                	lw	a5,0(a1)
  a2a3f6:	842a                	mv	s0,a0
  a2a3f8:	8782                	jr	a5
  a2a3fa:	00a0 5648 049f      	l.li	s1,0xa05648
  a2a400:	000107a3          	sb	zero,15(sp)
  a2a404:	4ca494db          	muliadd	s1,s1,a0,76
  a2a408:	249e                	lhu	a5,8(s1)
  a2a40a:	eb81                	bnez	a5,a2a41a <uart_evt_callback+0x3e>
  a2a40c:	4601                	li	a2,0
  a2a40e:	4589                	li	a1,2
  a2a410:	8522                	mv	a0,s0
  a2a412:	104010ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a416:	4501                	li	a0,0
  a2a418:	8154                	popret	{ra,s0-s3},48
  a2a41a:	00f10613          	addi	a2,sp,15
  a2a41e:	459d                	li	a1,7
  a2a420:	8522                	mv	a0,s0
  a2a422:	0f4010ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a426:	00f14783          	lbu	a5,15(sp)
  a2a42a:	f7f5                	bnez	a5,a2a416 <uart_evt_callback+0x3a>
  a2a42c:	8522                	mv	a0,s0
  a2a42e:	3701                	jal	ra,a2a32e <uart_helper_send_next_char>
  a2a430:	d56d                	beqz	a0,a2a41a <uart_evt_callback+0x3e>
  a2a432:	8522                	mv	a0,s0
  a2a434:	3d79                	jal	ra,a2a2d2 <uart_helper_invoke_current_fragment_callback>
  a2a436:	8522                	mv	a0,s0
  a2a438:	3d65                	jal	ra,a2a2f0 <uart_helper_move_to_next_fragment>
  a2a43a:	b7f9                	j	a2a408 <uart_evt_callback+0x2c>
  a2a43c:	00f10613          	addi	a2,sp,15
  a2a440:	45a5                	li	a1,9
  a2a442:	00a0 5600 091f      	l.li	s2,0xa05600
  a2a448:	00010723          	sb	zero,14(sp)
  a2a44c:	000107a3          	sb	zero,15(sp)
  a2a450:	4981                	li	s3,0
  a2a452:	0c4010ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a456:	188914db          	muliadd	s1,s2,s0,24
  a2a45a:	00f14783          	lbu	a5,15(sp)
  a2a45e:	c78d                	beqz	a5,a2a488 <uart_evt_callback+0xac>
  a2a460:	188917db          	muliadd	a5,s2,s0,24
  a2a464:	2bce                	lhu	a1,20(a5)
  a2a466:	d9c5                	beqz	a1,a2a416 <uart_evt_callback+0x3a>
  a2a468:	2bf8                	lbu	a4,22(a5)
  a2a46a:	8b09                	andi	a4,a4,2
  a2a46c:	d74d                	beqz	a4,a2a416 <uart_evt_callback+0x3a>
  a2a46e:	2bba                	lhu	a4,18(a5)
  a2a470:	fae9e3e3          	bltu	s3,a4,a2a416 <uart_evt_callback+0x3a>
  a2a474:	4398                	lw	a4,0(a5)
  a2a476:	c701                	beqz	a4,a2a47e <uart_evt_callback+0xa2>
  a2a478:	47c8                	lw	a0,12(a5)
  a2a47a:	4601                	li	a2,0
  a2a47c:	9702                	jalr	a4
  a2a47e:	1889145b          	muliadd	s0,s2,s0,24
  a2a482:	00041a23          	sh	zero,20(s0)
  a2a486:	bf41                	j	a2a416 <uart_evt_callback+0x3a>
  a2a488:	4605                	li	a2,1
  a2a48a:	00e10593          	addi	a1,sp,14
  a2a48e:	8522                	mv	a0,s0
  a2a490:	074010ef          	jal	ra,a2b504 <hal_uart_read>
  a2a494:	409c                	lw	a5,0(s1)
  a2a496:	0985                	addi	s3,s3,1 # ffff0001 <_gp_+0xff5bb845>
  a2a498:	09c2                	slli	s3,s3,0x10
  a2a49a:	0109d993          	srli	s3,s3,0x10
  a2a49e:	eb81                	bnez	a5,a2a4ae <uart_evt_callback+0xd2>
  a2a4a0:	00f10613          	addi	a2,sp,15
  a2a4a4:	45a5                	li	a1,9
  a2a4a6:	8522                	mv	a0,s0
  a2a4a8:	06e010ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a4ac:	b77d                	j	a2a45a <uart_evt_callback+0x7e>
  a2a4ae:	28d6                	lhu	a3,20(s1)
  a2a4b0:	44dc                	lw	a5,12(s1)
  a2a4b2:	00e14703          	lbu	a4,14(sp)
  a2a4b6:	97b6                	add	a5,a5,a3
  a2a4b8:	a398                	sb	a4,0(a5)
  a2a4ba:	28ce                	lhu	a1,20(s1)
  a2a4bc:	289e                	lhu	a5,16(s1)
  a2a4be:	0585                	addi	a1,a1,1
  a2a4c0:	9da1                	uxth	a1
  a2a4c2:	a8ce                	sh	a1,20(s1)
  a2a4c4:	fcf5eee3          	bltu	a1,a5,a2a4a0 <uart_evt_callback+0xc4>
  a2a4c8:	409c                	lw	a5,0(s1)
  a2a4ca:	44c8                	lw	a0,12(s1)
  a2a4cc:	4601                	li	a2,0
  a2a4ce:	9782                	jalr	a5
  a2a4d0:	00049a23          	sh	zero,20(s1)
  a2a4d4:	b7f1                	j	a2a4a0 <uart_evt_callback+0xc4>
  a2a4d6:	00f10613          	addi	a2,sp,15
  a2a4da:	45a5                	li	a1,9
  a2a4dc:	00a0 5600 091f      	l.li	s2,0xa05600
  a2a4e2:	00010723          	sb	zero,14(sp)
  a2a4e6:	000107a3          	sb	zero,15(sp)
  a2a4ea:	4981                	li	s3,0
  a2a4ec:	02a010ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a4f0:	188914db          	muliadd	s1,s2,s0,24
  a2a4f4:	00f14783          	lbu	a5,15(sp)
  a2a4f8:	cb85                	beqz	a5,a2a528 <uart_evt_callback+0x14c>
  a2a4fa:	1889175b          	muliadd	a4,s2,s0,24
  a2a4fe:	2b4e                	lhu	a1,20(a4)
  a2a500:	f0058be3          	beqz	a1,a2a416 <uart_evt_callback+0x3a>
  a2a504:	2b7c                	lbu	a5,22(a4)
  a2a506:	0017f693          	andi	a3,a5,1
  a2a50a:	e699                	bnez	a3,a2a518 <uart_evt_callback+0x13c>
  a2a50c:	8b89                	andi	a5,a5,2
  a2a50e:	f00784e3          	beqz	a5,a2a416 <uart_evt_callback+0x3a>
  a2a512:	2b3e                	lhu	a5,18(a4)
  a2a514:	f0f9e1e3          	bltu	s3,a5,a2a416 <uart_evt_callback+0x3a>
  a2a518:	1889175b          	muliadd	a4,s2,s0,24
  a2a51c:	431c                	lw	a5,0(a4)
  a2a51e:	d3a5                	beqz	a5,a2a47e <uart_evt_callback+0xa2>
  a2a520:	4748                	lw	a0,12(a4)
  a2a522:	4601                	li	a2,0
  a2a524:	9782                	jalr	a5
  a2a526:	bfa1                	j	a2a47e <uart_evt_callback+0xa2>
  a2a528:	4605                	li	a2,1
  a2a52a:	00e10593          	addi	a1,sp,14
  a2a52e:	8522                	mv	a0,s0
  a2a530:	7d5000ef          	jal	ra,a2b504 <hal_uart_read>
  a2a534:	409c                	lw	a5,0(s1)
  a2a536:	0985                	addi	s3,s3,1
  a2a538:	09c2                	slli	s3,s3,0x10
  a2a53a:	0109d993          	srli	s3,s3,0x10
  a2a53e:	eb81                	bnez	a5,a2a54e <uart_evt_callback+0x172>
  a2a540:	00f10613          	addi	a2,sp,15
  a2a544:	45a5                	li	a1,9
  a2a546:	8522                	mv	a0,s0
  a2a548:	7cf000ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a54c:	b765                	j	a2a4f4 <uart_evt_callback+0x118>
  a2a54e:	28d6                	lhu	a3,20(s1)
  a2a550:	44dc                	lw	a5,12(s1)
  a2a552:	00e14703          	lbu	a4,14(sp)
  a2a556:	97b6                	add	a5,a5,a3
  a2a558:	a398                	sb	a4,0(a5)
  a2a55a:	28ce                	lhu	a1,20(s1)
  a2a55c:	289e                	lhu	a5,16(s1)
  a2a55e:	0585                	addi	a1,a1,1
  a2a560:	9da1                	uxth	a1
  a2a562:	a8ce                	sh	a1,20(s1)
  a2a564:	fcf5eee3          	bltu	a1,a5,a2a540 <uart_evt_callback+0x164>
  a2a568:	409c                	lw	a5,0(s1)
  a2a56a:	c781                	beqz	a5,a2a572 <uart_evt_callback+0x196>
  a2a56c:	44c8                	lw	a0,12(s1)
  a2a56e:	4601                	li	a2,0
  a2a570:	9782                	jalr	a5
  a2a572:	00049a23          	sh	zero,20(s1)
  a2a576:	b7e9                	j	a2a540 <uart_evt_callback+0x164>
  a2a578:	00a0 5600 079f      	l.li	a5,0xa05600
  a2a57e:	18a797db          	muliadd	a5,a5,a0,24
  a2a582:	43dc                	lw	a5,4(a5)
  a2a584:	c781                	beqz	a5,a2a58c <uart_evt_callback+0x1b0>
  a2a586:	4581                	li	a1,0
  a2a588:	4501                	li	a0,0
  a2a58a:	9782                	jalr	a5
  a2a58c:	8522                	mv	a0,s0
  a2a58e:	33f9                	jal	ra,a2a35c <uart_error_isr>
  a2a590:	b559                	j	a2a416 <uart_evt_callback+0x3a>
  a2a592:	00a0 5600 079f      	l.li	a5,0xa05600
  a2a598:	18a797db          	muliadd	a5,a5,a0,24
  a2a59c:	479c                	lw	a5,8(a5)
  a2a59e:	b7dd                	j	a2a584 <uart_evt_callback+0x1a8>

00a2a5a0 <uapi_uart_init>:
  a2a5a0:	8148                	push	{ra,s0-s2},-32
  a2a5a2:	4789                	li	a5,2
  a2a5a4:	0aa7e563          	bltu	a5,a0,a2a64e <uapi_uart_init+0xae>
  a2a5a8:	c1dd                	beqz	a1,a2a64e <uapi_uart_init+0xae>
  a2a5aa:	0015c803          	lbu	a6,1(a1)
  a2a5ae:	07200693          	li	a3,114
  a2a5b2:	0906ee63          	bltu	a3,a6,a2a64e <uapi_uart_init+0xae>
  a2a5b6:	0005c803          	lbu	a6,0(a1)
  a2a5ba:	0906ea63          	bltu	a3,a6,a2a64e <uapi_uart_init+0xae>
  a2a5be:	ca41                	beqz	a2,a2a64e <uapi_uart_init+0xae>
  a2a5c0:	2254                	lbu	a3,4(a2)
  a2a5c2:	0426f33b          	bgeui	a3,4,a2a64e <uapi_uart_init+0xae>
  a2a5c6:	2274                	lbu	a3,6(a2)
  a2a5c8:	08d7e363          	bltu	a5,a3,a2a64e <uapi_uart_init+0xae>
  a2a5cc:	325c                	lbu	a5,5(a2)
  a2a5ce:	0227f03b          	bgeui	a5,2,a2a64e <uapi_uart_init+0xae>
  a2a5d2:	00a05937          	lui	s2,0xa05
  a2a5d6:	5fc90793          	addi	a5,s2,1532 # a055fc <g_uart_inited>
  a2a5da:	97aa                	add	a5,a5,a0
  a2a5dc:	239c                	lbu	a5,0(a5)
  a2a5de:	84aa                	mv	s1,a0
  a2a5e0:	5fc90913          	addi	s2,s2,1532
  a2a5e4:	4401                	li	s0,0
  a2a5e6:	e3b5                	bnez	a5,a2a64a <uapi_uart_init+0xaa>
  a2a5e8:	843a                	mv	s0,a4
  a2a5ea:	c632                	sw	a2,12(sp)
  a2a5ec:	c42e                	sw	a1,8(sp)
  a2a5ee:	243010ef          	jal	ra,a2c030 <uart_port_config_pinmux>
  a2a5f2:	c035                	beqz	s0,a2a656 <uapi_uart_init+0xb6>
  a2a5f4:	4018                	lw	a4,0(s0)
  a2a5f6:	c325                	beqz	a4,a2a656 <uapi_uart_init+0xb6>
  a2a5f8:	405c                	lw	a5,4(s0)
  a2a5fa:	cfb1                	beqz	a5,a2a656 <uapi_uart_init+0xb6>
  a2a5fc:	04c00593          	li	a1,76
  a2a600:	02b485b3          	mul	a1,s1,a1
  a2a604:	00a0 5600 079f      	l.li	a5,0xa05600
  a2a60a:	189797db          	muliadd	a5,a5,s1,24
  a2a60e:	c7d8                	sw	a4,12(a5)
  a2a610:	4058                	lw	a4,4(s0)
  a2a612:	46b2                	lw	a3,12(sp)
  a2a614:	4622                	lw	a2,8(sp)
  a2a616:	ab9a                	sh	a4,16(a5)
  a2a618:	00a0 5648 079f      	l.li	a5,0xa05648
  a2a61e:	8526                	mv	a0,s1
  a2a620:	00c58713          	addi	a4,a1,12
  a2a624:	973e                	add	a4,a4,a5
  a2a626:	97ae                	add	a5,a5,a1
  a2a628:	c398                	sw	a4,0(a5)
  a2a62a:	c3d8                	sw	a4,4(a5)
  a2a62c:	00a2 a3dc 059f      	l.li	a1,0xa2a3dc
  a2a632:	4711                	li	a4,4
  a2a634:	685000ef          	jal	ra,a2b4b8 <hal_uart_init>
  a2a638:	842a                	mv	s0,a0
  a2a63a:	e901                	bnez	a0,a2a64a <uapi_uart_init+0xaa>
  a2a63c:	9926                	add	s2,s2,s1
  a2a63e:	4785                	li	a5,1
  a2a640:	8526                	mv	a0,s1
  a2a642:	00f90023          	sb	a5,0(s2)
  a2a646:	1fb010ef          	jal	ra,a2c040 <uart_port_register_irq>
  a2a64a:	8522                	mv	a0,s0
  a2a64c:	8144                	popret	{ra,s0-s2},32
  a2a64e:	80000437          	lui	s0,0x80000
  a2a652:	0405                	addi	s0,s0,1 # 80000001 <_gp_+0x7f5cb845>
  a2a654:	bfdd                	j	a2a64a <uapi_uart_init+0xaa>
  a2a656:	8000 1044 041f      	l.li	s0,0x80001044
  a2a65c:	b7fd                	j	a2a64a <uapi_uart_init+0xaa>

00a2a65e <uapi_uart_write>:
  a2a65e:	8178                	push	{ra,s0-s5},-48
  a2a660:	000107a3          	sb	zero,15(sp)
  a2a664:	0315773b          	bgeui	a0,3,a2a6c0 <uapi_uart_write+0x62>
  a2a668:	cda1                	beqz	a1,a2a6c0 <uapi_uart_write+0x62>
  a2a66a:	ca39                	beqz	a2,a2a6c0 <uapi_uart_write+0x62>
  a2a66c:	00a0 55fc 079f      	l.li	a5,0xa055fc
  a2a672:	97aa                	add	a5,a5,a0
  a2a674:	239c                	lbu	a5,0(a5)
  a2a676:	84aa                	mv	s1,a0
  a2a678:	cba1                	beqz	a5,a2a6c8 <uapi_uart_write+0x6a>
  a2a67a:	89ae                	mv	s3,a1
  a2a67c:	8932                	mv	s2,a2
  a2a67e:	227010ef          	jal	ra,a2c0a4 <uart_porting_lock>
  a2a682:	8aaa                	mv	s5,a0
  a2a684:	994e                	add	s2,s2,s3
  a2a686:	8a4e                	mv	s4,s3
  a2a688:	413a0433          	sub	s0,s4,s3
  a2a68c:	012a1863          	bne	s4,s2,a2a69c <uapi_uart_write+0x3e>
  a2a690:	85d6                	mv	a1,s5
  a2a692:	8526                	mv	a0,s1
  a2a694:	215010ef          	jal	ra,a2c0a8 <uart_porting_unlock>
  a2a698:	8522                	mv	a0,s0
  a2a69a:	8174                	popret	{ra,s0-s5},48
  a2a69c:	00f10613          	addi	a2,sp,15
  a2a6a0:	459d                	li	a1,7
  a2a6a2:	8526                	mv	a0,s1
  a2a6a4:	673000ef          	jal	ra,a2b516 <hal_uart_ctrl>
  a2a6a8:	00f14783          	lbu	a5,15(sp)
  a2a6ac:	f3e5                	bnez	a5,a2a68c <uapi_uart_write+0x2e>
  a2a6ae:	85d2                	mv	a1,s4
  a2a6b0:	001a0413          	addi	s0,s4,1
  a2a6b4:	4605                	li	a2,1
  a2a6b6:	8526                	mv	a0,s1
  a2a6b8:	63b000ef          	jal	ra,a2b4f2 <hal_uart_write>
  a2a6bc:	8a22                	mv	s4,s0
  a2a6be:	b7e9                	j	a2a688 <uapi_uart_write+0x2a>
  a2a6c0:	80000437          	lui	s0,0x80000
  a2a6c4:	0405                	addi	s0,s0,1 # 80000001 <_gp_+0x7f5cb845>
  a2a6c6:	bfc9                	j	a2a698 <uapi_uart_write+0x3a>
  a2a6c8:	8000 1040 041f      	l.li	s0,0x80001040
  a2a6ce:	b7e9                	j	a2a698 <uapi_uart_write+0x3a>

00a2a6d0 <uapi_watchdog_init>:
  a2a6d0:	8058                	push	{ra,s0-s3},-32
  a2a6d2:	89aa                	mv	s3,a0
  a2a6d4:	211010ef          	jal	ra,a2c0e4 <watchdog_port_register_hal_funcs>
  a2a6d8:	21d010ef          	jal	ra,a2c0f4 <watchdog_port_register_irq>
  a2a6dc:	9f8ff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a6e0:	84aa                	mv	s1,a0
  a2a6e2:	26e010ef          	jal	ra,a2b950 <hal_watchdog_get_funcs>
  a2a6e6:	411c                	lw	a5,0(a0)
  a2a6e8:	00a0 572c 091f      	l.li	s2,0xa0572c
  a2a6ee:	00a92023          	sw	a0,0(s2)
  a2a6f2:	9782                	jalr	a5
  a2a6f4:	c519                	beqz	a0,a2a702 <uapi_watchdog_init+0x32>
  a2a6f6:	842a                	mv	s0,a0
  a2a6f8:	8526                	mv	a0,s1
  a2a6fa:	9e2ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a6fe:	8522                	mv	a0,s0
  a2a700:	8054                	popret	{ra,s0-s3},32
  a2a702:	00092783          	lw	a5,0(s2)
  a2a706:	854e                	mv	a0,s3
  a2a708:	479c                	lw	a5,8(a5)
  a2a70a:	9782                	jalr	a5
  a2a70c:	842a                	mv	s0,a0
  a2a70e:	8526                	mv	a0,s1
  a2a710:	9ccff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a714:	f46d                	bnez	s0,a2a6fe <uapi_watchdog_init+0x2e>
  a2a716:	00a0 5731 079f      	l.li	a5,0xa05731
  a2a71c:	4705                	li	a4,1
  a2a71e:	a398                	sb	a4,0(a5)
  a2a720:	bff9                	j	a2a6fe <uapi_watchdog_init+0x2e>

00a2a722 <uapi_watchdog_enable>:
  a2a722:	00a0 5731 079f      	l.li	a5,0xa05731
  a2a728:	239c                	lbu	a5,0(a5)
  a2a72a:	cb9d                	beqz	a5,a2a760 <uapi_watchdog_enable+0x3e>
  a2a72c:	8048                	push	{ra,s0-s2},-16
  a2a72e:	4485                	li	s1,1
  a2a730:	57fd                	li	a5,-1
  a2a732:	02a4e563          	bltu	s1,a0,a2a75c <uapi_watchdog_enable+0x3a>
  a2a736:	842a                	mv	s0,a0
  a2a738:	99cff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a73c:	00a0 572c 079f      	l.li	a5,0xa0572c
  a2a742:	439c                	lw	a5,0(a5)
  a2a744:	892a                	mv	s2,a0
  a2a746:	8522                	mv	a0,s0
  a2a748:	4b9c                	lw	a5,16(a5)
  a2a74a:	9782                	jalr	a5
  a2a74c:	854a                	mv	a0,s2
  a2a74e:	98eff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a752:	00a0 5730 079f      	l.li	a5,0xa05730
  a2a758:	a384                	sb	s1,0(a5)
  a2a75a:	4781                	li	a5,0
  a2a75c:	853e                	mv	a0,a5
  a2a75e:	8044                	popret	{ra,s0-s2},16
  a2a760:	57fd                	li	a5,-1
  a2a762:	853e                	mv	a0,a5
  a2a764:	8082                	ret

00a2a766 <uapi_watchdog_disable>:
  a2a766:	00a0 5731 079f      	l.li	a5,0xa05731
  a2a76c:	239c                	lbu	a5,0(a5)
  a2a76e:	e399                	bnez	a5,a2a774 <uapi_watchdog_disable+0xe>
  a2a770:	557d                	li	a0,-1
  a2a772:	8082                	ret
  a2a774:	8118                	push	{ra},-32
  a2a776:	95eff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a77a:	00a0 572c 079f      	l.li	a5,0xa0572c
  a2a780:	439c                	lw	a5,0(a5)
  a2a782:	c62a                	sw	a0,12(sp)
  a2a784:	4bdc                	lw	a5,20(a5)
  a2a786:	9782                	jalr	a5
  a2a788:	4532                	lw	a0,12(sp)
  a2a78a:	952ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a78e:	00a0 5730 079f      	l.li	a5,0xa05730
  a2a794:	00078023          	sb	zero,0(a5)
  a2a798:	4501                	li	a0,0
  a2a79a:	8114                	popret	{ra},32

00a2a79c <uapi_watchdog_deinit>:
  a2a79c:	00a057b7          	lui	a5,0xa05
  a2a7a0:	7317c703          	lbu	a4,1841(a5) # a05731 <g_watchdog_inited>
  a2a7a4:	c71d                	beqz	a4,a2a7d2 <uapi_watchdog_deinit+0x36>
  a2a7a6:	8028                	push	{ra,s0},-16
  a2a7a8:	73178413          	addi	s0,a5,1841
  a2a7ac:	00a0 5730 079f      	l.li	a5,0xa05730
  a2a7b2:	239c                	lbu	a5,0(a5)
  a2a7b4:	c391                	beqz	a5,a2a7b8 <uapi_watchdog_deinit+0x1c>
  a2a7b6:	3f45                	jal	ra,a2a766 <uapi_watchdog_disable>
  a2a7b8:	00a0 572c 079f      	l.li	a5,0xa0572c
  a2a7be:	439c                	lw	a5,0(a5)
  a2a7c0:	c399                	beqz	a5,a2a7c6 <uapi_watchdog_deinit+0x2a>
  a2a7c2:	43dc                	lw	a5,4(a5)
  a2a7c4:	9782                	jalr	a5
  a2a7c6:	12b010ef          	jal	ra,a2c0f0 <watchdog_port_unregister_hal_funcs>
  a2a7ca:	4501                	li	a0,0
  a2a7cc:	00040023          	sb	zero,0(s0)
  a2a7d0:	8024                	popret	{ra,s0},16
  a2a7d2:	4501                	li	a0,0
  a2a7d4:	8082                	ret

00a2a7d6 <uapi_watchdog_kick>:
  a2a7d6:	00a0 5730 079f      	l.li	a5,0xa05730
  a2a7dc:	239c                	lbu	a5,0(a5)
  a2a7de:	e399                	bnez	a5,a2a7e4 <uapi_watchdog_kick+0xe>
  a2a7e0:	557d                	li	a0,-1
  a2a7e2:	8082                	ret
  a2a7e4:	8118                	push	{ra},-32
  a2a7e6:	8eeff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a7ea:	00a0 572c 079f      	l.li	a5,0xa0572c
  a2a7f0:	439c                	lw	a5,0(a5)
  a2a7f2:	c62a                	sw	a0,12(sp)
  a2a7f4:	4f9c                	lw	a5,24(a5)
  a2a7f6:	9782                	jalr	a5
  a2a7f8:	4532                	lw	a0,12(sp)
  a2a7fa:	8e2ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a7fe:	4501                	li	a0,0
  a2a800:	8114                	popret	{ra},32

00a2a802 <uapi_efuse_init>:
  a2a802:	8018                	push	{ra},-16
  a2a804:	0b4020ef          	jal	ra,a2c8b8 <efuse_port_register_hal_funcs>
  a2a808:	28c1                	jal	ra,a2a8d8 <hal_efuse_get_funcs>
  a2a80a:	00052303          	lw	t1,0(a0)
  a2a80e:	8010                	pop	{ra},16
  a2a810:	8302                	jr	t1

00a2a812 <uapi_efuse_read_bit>:
  a2a812:	8168                	push	{ra,s0-s4},-48
  a2a814:	000107a3          	sb	zero,15(sp)
  a2a818:	547d                	li	s0,-1
  a2a81a:	0ff00793          	li	a5,255
  a2a81e:	02b7ee63          	bltu	a5,a1,a2a85a <uapi_efuse_read_bit+0x48>
  a2a822:	cd05                	beqz	a0,a2a85a <uapi_efuse_read_bit+0x48>
  a2a824:	08067dbb          	bgeui	a2,8,a2a85a <uapi_efuse_read_bit+0x48>
  a2a828:	842e                	mv	s0,a1
  a2a82a:	8932                	mv	s2,a2
  a2a82c:	84aa                	mv	s1,a0
  a2a82e:	206d                	jal	ra,a2a8d8 <hal_efuse_get_funcs>
  a2a830:	8a2a                	mv	s4,a0
  a2a832:	8a2ff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a836:	010a2783          	lw	a5,16(s4)
  a2a83a:	89aa                	mv	s3,a0
  a2a83c:	00f10593          	addi	a1,sp,15
  a2a840:	8522                	mv	a0,s0
  a2a842:	9782                	jalr	a5
  a2a844:	842a                	mv	s0,a0
  a2a846:	ed01                	bnez	a0,a2a85e <uapi_efuse_read_bit+0x4c>
  a2a848:	00f14783          	lbu	a5,15(sp)
  a2a84c:	854e                	mv	a0,s3
  a2a84e:	4127d633          	sra	a2,a5,s2
  a2a852:	8a05                	andi	a2,a2,1
  a2a854:	a090                	sb	a2,0(s1)
  a2a856:	886ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a85a:	8522                	mv	a0,s0
  a2a85c:	8164                	popret	{ra,s0-s4},48
  a2a85e:	854e                	mv	a0,s3
  a2a860:	87cff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a864:	547d                	li	s0,-1
  a2a866:	bfd5                	j	a2a85a <uapi_efuse_read_bit+0x48>

00a2a868 <uapi_efuse_read_buffer>:
  a2a868:	8088                	push	{ra,s0-s6},-32
  a2a86a:	547d                	li	s0,-1
  a2a86c:	c231                	beqz	a2,a2a8b0 <uapi_efuse_read_buffer+0x48>
  a2a86e:	0ff00793          	li	a5,255
  a2a872:	02b7ef63          	bltu	a5,a1,a2a8b0 <uapi_efuse_read_buffer+0x48>
  a2a876:	00c587b3          	add	a5,a1,a2
  a2a87a:	10000713          	li	a4,256
  a2a87e:	02f76963          	bltu	a4,a5,a2a8b0 <uapi_efuse_read_buffer+0x48>
  a2a882:	c51d                	beqz	a0,a2a8b0 <uapi_efuse_read_buffer+0x48>
  a2a884:	8932                	mv	s2,a2
  a2a886:	89ae                	mv	s3,a1
  a2a888:	8a2a                	mv	s4,a0
  a2a88a:	20b9                	jal	ra,a2a8d8 <hal_efuse_get_funcs>
  a2a88c:	8b2a                	mv	s6,a0
  a2a88e:	846ff0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2a892:	8aaa                	mv	s5,a0
  a2a894:	4481                	li	s1,0
  a2a896:	010b2783          	lw	a5,16(s6)
  a2a89a:	009a05b3          	add	a1,s4,s1
  a2a89e:	00998533          	add	a0,s3,s1
  a2a8a2:	9782                	jalr	a5
  a2a8a4:	842a                	mv	s0,a0
  a2a8a6:	c519                	beqz	a0,a2a8b4 <uapi_efuse_read_buffer+0x4c>
  a2a8a8:	8556                	mv	a0,s5
  a2a8aa:	832ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a8ae:	547d                	li	s0,-1
  a2a8b0:	8522                	mv	a0,s0
  a2a8b2:	8084                	popret	{ra,s0-s6},32
  a2a8b4:	0485                	addi	s1,s1,1
  a2a8b6:	ff24e0e3          	bltu	s1,s2,a2a896 <uapi_efuse_read_buffer+0x2e>
  a2a8ba:	8556                	mv	a0,s5
  a2a8bc:	820ff0ef          	jal	ra,a298dc <osal_irq_restore>
  a2a8c0:	bfc5                	j	a2a8b0 <uapi_efuse_read_buffer+0x48>

00a2a8c2 <hal_efuse_register_funcs>:
  a2a8c2:	c519                	beqz	a0,a2a8d0 <hal_efuse_register_funcs+0xe>
  a2a8c4:	00a0 5734 079f      	l.li	a5,0xa05734
  a2a8ca:	c388                	sw	a0,0(a5)
  a2a8cc:	4501                	li	a0,0
  a2a8ce:	8082                	ret
  a2a8d0:	80000537          	lui	a0,0x80000
  a2a8d4:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2a8d6:	8082                	ret

00a2a8d8 <hal_efuse_get_funcs>:
  a2a8d8:	00a0 5734 079f      	l.li	a5,0xa05734
  a2a8de:	4388                	lw	a0,0(a5)
  a2a8e0:	8082                	ret

00a2a8e2 <hal_efuse_deinit>:
  a2a8e2:	00a0 5738 079f      	l.li	a5,0xa05738
  a2a8e8:	00078023          	sb	zero,0(a5)
  a2a8ec:	8082                	ret

00a2a8ee <hal_efuse_get_writeread_addr>:
  a2a8ee:	8038                	push	{ra,s0-s1},-16
  a2a8f0:	84aa                	mv	s1,a0
  a2a8f2:	7d3010ef          	jal	ra,a2c8c4 <hal_efuse_get_region>
  a2a8f6:	842a                	mv	s0,a0
  a2a8f8:	8526                	mv	a0,s1
  a2a8fa:	7d1010ef          	jal	ra,a2c8ca <hal_efuse_get_byte_offset>
  a2a8fe:	00a3 3f40 079f      	l.li	a5,0xa33f40
  a2a904:	0487879b          	addshf	a5,a5,s0,sll,2
  a2a908:	439c                	lw	a5,0(a5)
  a2a90a:	8105                	srli	a0,a0,0x1
  a2a90c:	9d21                	uxth	a0
  a2a90e:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2a912:	8034                	popret	{ra,s0-s1},16

00a2a914 <hal_efuse_init>:
  a2a914:	8018                	push	{ra},-16
  a2a916:	4501                	li	a0,0
  a2a918:	2255                	jal	ra,a2aabc <hal_efuse_regs_init>
  a2a91a:	00a057b7          	lui	a5,0xa05
  a2a91e:	7407a703          	lw	a4,1856(a5) # a05740 <g_efuse_regs>
  a2a922:	00a3 3dd4 069f      	l.li	a3,0xa33dd4
  a2a928:	2294                	lbu	a3,0(a3)
  a2a92a:	435c                	lw	a5,4(a4)
  a2a92c:	4501                	li	a0,0
  a2a92e:	f007f793          	andi	a5,a5,-256
  a2a932:	8fd5                	or	a5,a5,a3
  a2a934:	c35c                	sw	a5,4(a4)
  a2a936:	00a0 5738 079f      	l.li	a5,0xa05738
  a2a93c:	4705                	li	a4,1
  a2a93e:	a398                	sb	a4,0(a5)
  a2a940:	8014                	popret	{ra},16

00a2a942 <hal_efuse_read_byte>:
  a2a942:	00a0 5738 079f      	l.li	a5,0xa05738
  a2a948:	239c                	lbu	a5,0(a5)
  a2a94a:	c7a1                	beqz	a5,a2a992 <hal_efuse_read_byte+0x50>
  a2a94c:	8048                	push	{ra,s0-s2},-16
  a2a94e:	842a                	mv	s0,a0
  a2a950:	0ff00793          	li	a5,255
  a2a954:	04a7e363          	bltu	a5,a0,a2a99a <hal_efuse_read_byte+0x58>
  a2a958:	c1a9                	beqz	a1,a2a99a <hal_efuse_read_byte+0x58>
  a2a95a:	84ae                	mv	s1,a1
  a2a95c:	3f49                	jal	ra,a2a8ee <hal_efuse_get_writeread_addr>
  a2a95e:	892a                	mv	s2,a0
  a2a960:	8522                	mv	a0,s0
  a2a962:	763010ef          	jal	ra,a2c8c4 <hal_efuse_get_region>
  a2a966:	00a0 5740 079f      	l.li	a5,0xa05740
  a2a96c:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2a970:	4118                	lw	a4,0(a0)
  a2a972:	76c1                	lui	a3,0xffff0
  a2a974:	8805                	andi	s0,s0,1
  a2a976:	431c                	lw	a5,0(a4)
  a2a978:	8ff5                	and	a5,a5,a3
  a2a97a:	0000 5a5a 069f      	l.li	a3,0x5a5a
  a2a980:	8fd5                	or	a5,a5,a3
  a2a982:	c31c                	sw	a5,0(a4)
  a2a984:	00092783          	lw	a5,0(s2)
  a2a988:	c011                	beqz	s0,a2a98c <hal_efuse_read_byte+0x4a>
  a2a98a:	83a1                	srli	a5,a5,0x8
  a2a98c:	a09c                	sb	a5,0(s1)
  a2a98e:	4501                	li	a0,0
  a2a990:	8044                	popret	{ra,s0-s2},16
  a2a992:	8000 1391 051f      	l.li	a0,0x80001391
  a2a998:	8082                	ret
  a2a99a:	80000537          	lui	a0,0x80000
  a2a99e:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2a9a0:	bfc5                	j	a2a990 <hal_efuse_read_byte+0x4e>

00a2a9a2 <hal_efuse_get_die_id>:
  a2a9a2:	00a0 5738 079f      	l.li	a5,0xa05738
  a2a9a8:	239c                	lbu	a5,0(a5)
  a2a9aa:	ef99                	bnez	a5,a2a9c8 <hal_efuse_get_die_id+0x26>
  a2a9ac:	8000 1391 051f      	l.li	a0,0x80001391
  a2a9b2:	8082                	ret
  a2a9b4:	008905b3          	add	a1,s2,s0
  a2a9b8:	8522                	mv	a0,s0
  a2a9ba:	3761                	jal	ra,a2a942 <hal_efuse_read_byte>
  a2a9bc:	e919                	bnez	a0,a2a9d2 <hal_efuse_get_die_id+0x30>
  a2a9be:	0405                	addi	s0,s0,1
  a2a9c0:	fe946ae3          	bltu	s0,s1,a2a9b4 <hal_efuse_get_die_id+0x12>
  a2a9c4:	4501                	li	a0,0
  a2a9c6:	8044                	popret	{ra,s0-s2},16
  a2a9c8:	8048                	push	{ra,s0-s2},-16
  a2a9ca:	892a                	mv	s2,a0
  a2a9cc:	84ae                	mv	s1,a1
  a2a9ce:	4401                	li	s0,0
  a2a9d0:	bfc5                	j	a2a9c0 <hal_efuse_get_die_id+0x1e>
  a2a9d2:	557d                	li	a0,-1
  a2a9d4:	bfcd                	j	a2a9c6 <hal_efuse_get_die_id+0x24>

00a2a9d6 <hal_efuse_write_operation>:
  a2a9d6:	00a0 5738 079f      	l.li	a5,0xa05738
  a2a9dc:	239c                	lbu	a5,0(a5)
  a2a9de:	c3c1                	beqz	a5,a2aa5e <hal_efuse_write_operation+0x88>
  a2a9e0:	8058                	push	{ra,s0-s3},-32
  a2a9e2:	842a                	mv	s0,a0
  a2a9e4:	0ff00793          	li	a5,255
  a2a9e8:	06a7ef63          	bltu	a5,a0,a2aa66 <hal_efuse_write_operation+0x90>
  a2a9ec:	00a054b7          	lui	s1,0xa05
  a2a9f0:	892e                	mv	s2,a1
  a2a9f2:	74048993          	addi	s3,s1,1856 # a05740 <g_efuse_regs>
  a2a9f6:	6cf010ef          	jal	ra,a2c8c4 <hal_efuse_get_region>
  a2a9fa:	04a9851b          	addshf	a0,s3,a0,sll,2
  a2a9fe:	4118                	lw	a4,0(a0)
  a2aa00:	76c1                	lui	a3,0xffff0
  a2aa02:	8522                	mv	a0,s0
  a2aa04:	431c                	lw	a5,0(a4)
  a2aa06:	74048493          	addi	s1,s1,1856
  a2aa0a:	8ff5                	and	a5,a5,a3
  a2aa0c:	0000 a5a5 069f      	l.li	a3,0xa5a5
  a2aa12:	8fd5                	or	a5,a5,a3
  a2aa14:	c31c                	sw	a5,0(a4)
  a2aa16:	6af010ef          	jal	ra,a2c8c4 <hal_efuse_get_region>
  a2aa1a:	04a9851b          	addshf	a0,s3,a0,sll,2
  a2aa1e:	4118                	lw	a4,0(a0)
  a2aa20:	06400513          	li	a0,100
  a2aa24:	475c                	lw	a5,12(a4)
  a2aa26:	0017e793          	ori	a5,a5,1
  a2aa2a:	c75c                	sw	a5,12(a4)
  a2aa2c:	869ff0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2aa30:	8522                	mv	a0,s0
  a2aa32:	3d75                	jal	ra,a2a8ee <hal_efuse_get_writeread_addr>
  a2aa34:	00147793          	andi	a5,s0,1
  a2aa38:	c391                	beqz	a5,a2aa3c <hal_efuse_write_operation+0x66>
  a2aa3a:	0922                	slli	s2,s2,0x8
  a2aa3c:	01252023          	sw	s2,0(a0)
  a2aa40:	8522                	mv	a0,s0
  a2aa42:	683010ef          	jal	ra,a2c8c4 <hal_efuse_get_region>
  a2aa46:	04a4849b          	addshf	s1,s1,a0,sll,2
  a2aa4a:	4098                	lw	a4,0(s1)
  a2aa4c:	06400513          	li	a0,100
  a2aa50:	475c                	lw	a5,12(a4)
  a2aa52:	9bf9                	andi	a5,a5,-2
  a2aa54:	c75c                	sw	a5,12(a4)
  a2aa56:	83fff0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2aa5a:	4501                	li	a0,0
  a2aa5c:	8054                	popret	{ra,s0-s3},32
  a2aa5e:	8000 1391 051f      	l.li	a0,0x80001391
  a2aa64:	8082                	ret
  a2aa66:	80000537          	lui	a0,0x80000
  a2aa6a:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2aa6c:	bfc5                	j	a2aa5c <hal_efuse_write_operation+0x86>

00a2aa6e <hal_efuse_write_buffer_operation>:
  a2aa6e:	00a0 5738 079f      	l.li	a5,0xa05738
  a2aa74:	239c                	lbu	a5,0(a5)
  a2aa76:	c795                	beqz	a5,a2aaa2 <hal_efuse_write_buffer_operation+0x34>
  a2aa78:	8058                	push	{ra,s0-s3},-32
  a2aa7a:	89b2                	mv	s3,a2
  a2aa7c:	842e                	mv	s0,a1
  a2aa7e:	84ae                	mv	s1,a1
  a2aa80:	40b50933          	sub	s2,a0,a1
  a2aa84:	408487b3          	sub	a5,s1,s0
  a2aa88:	0137e463          	bltu	a5,s3,a2aa90 <hal_efuse_write_buffer_operation+0x22>
  a2aa8c:	4501                	li	a0,0
  a2aa8e:	8054                	popret	{ra,s0-s3},32
  a2aa90:	208c                	lbu	a1,0(s1)
  a2aa92:	00990533          	add	a0,s2,s1
  a2aa96:	4601                	li	a2,0
  a2aa98:	3f3d                	jal	ra,a2a9d6 <hal_efuse_write_operation>
  a2aa9a:	0485                	addi	s1,s1,1
  a2aa9c:	d565                	beqz	a0,a2aa84 <hal_efuse_write_buffer_operation+0x16>
  a2aa9e:	557d                	li	a0,-1
  a2aaa0:	b7fd                	j	a2aa8e <hal_efuse_write_buffer_operation+0x20>
  a2aaa2:	8000 1391 051f      	l.li	a0,0x80001391
  a2aaa8:	8082                	ret

00a2aaaa <hal_efuse_set_clock_period>:
  a2aaaa:	00a3 3dd4 079f      	l.li	a5,0xa33dd4
  a2aab0:	c388                	sw	a0,0(a5)
  a2aab2:	8082                	ret

00a2aab4 <hal_efuse_funcs_get>:
  a2aab4:	00a3 3dd8 051f      	l.li	a0,0xa33dd8
  a2aaba:	8082                	ret

00a2aabc <hal_efuse_regs_init>:
  a2aabc:	02a04763          	bgtz	a0,a2aaea <hal_efuse_regs_init+0x2e>
  a2aac0:	050a                	slli	a0,a0,0x2
  a2aac2:	00a3 3f38 071f      	l.li	a4,0xa33f38
  a2aac8:	00a0 5740 079f      	l.li	a5,0xa05740
  a2aace:	97aa                	add	a5,a5,a0
  a2aad0:	953a                	add	a0,a0,a4
  a2aad2:	4118                	lw	a4,0(a0)
  a2aad4:	4501                	li	a0,0
  a2aad6:	c398                	sw	a4,0(a5)
  a2aad8:	00a347b7          	lui	a5,0xa34
  a2aadc:	f3c7a703          	lw	a4,-196(a5) # a33f3c <g_efuse_boot_done_addr>
  a2aae0:	00a0 573c 079f      	l.li	a5,0xa0573c
  a2aae6:	c398                	sw	a4,0(a5)
  a2aae8:	8082                	ret
  a2aaea:	557d                	li	a0,-1
  a2aaec:	8082                	ret

00a2aaee <hal_pmp_register_funcs>:
  a2aaee:	c519                	beqz	a0,a2aafc <hal_pmp_register_funcs+0xe>
  a2aaf0:	00a0 5744 079f      	l.li	a5,0xa05744
  a2aaf6:	c388                	sw	a0,0(a5)
  a2aaf8:	4501                	li	a0,0
  a2aafa:	8082                	ret
  a2aafc:	80000537          	lui	a0,0x80000
  a2ab00:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2ab02:	8082                	ret

00a2ab04 <hal_pmp_get_funcs>:
  a2ab04:	00a0 5744 079f      	l.li	a5,0xa05744
  a2ab0a:	4388                	lw	a0,0(a5)
  a2ab0c:	8082                	ret

00a2ab0e <hal_pmp_riscv31_config>:
  a2ab0e:	8048                	push	{ra,s0-s2},-16
  a2ab10:	00052903          	lw	s2,0(a0)
  a2ab14:	10097fbb          	bgeui	s2,16,a2ab52 <hal_pmp_riscv31_config+0x44>
  a2ab18:	414c                	lw	a1,4(a0)
  a2ab1a:	2500                	lbu	s0,8(a0)
  a2ab1c:	84aa                	mv	s1,a0
  a2ab1e:	854a                	mv	a0,s2
  a2ab20:	2089                	jal	ra,a2ab62 <hal_pmp_riscv31_regs_set_pmpaddr>
  a2ab22:	85a2                	mv	a1,s0
  a2ab24:	854a                	mv	a0,s2
  a2ab26:	28e9                	jal	ra,a2ac00 <hal_pmp_riscv31_regs_set_memxattr>
  a2ab28:	4480                	lw	s0,8(s1)
  a2ab2a:	854a                	mv	a0,s2
  a2ab2c:	8021                	srli	s0,s0,0x8
  a2ab2e:	881d                	andi	s0,s0,7
  a2ab30:	85a2                	mv	a1,s0
  a2ab32:	2a39                	jal	ra,a2ac50 <hal_pmp_riscv31_regs_set_pmpxcfg>
  a2ab34:	449c                	lw	a5,8(s1)
  a2ab36:	854a                	mv	a0,s2
  a2ab38:	0087d713          	srli	a4,a5,0x8
  a2ab3c:	8b61                	andi	a4,a4,24
  a2ab3e:	83a1                	srli	a5,a5,0x8
  a2ab40:	0807f593          	andi	a1,a5,128
  a2ab44:	8c59                	or	s0,s0,a4
  a2ab46:	8dc1                	or	a1,a1,s0
  a2ab48:	2221                	jal	ra,a2ac50 <hal_pmp_riscv31_regs_set_pmpxcfg>
  a2ab4a:	0ff0000f          	fence
  a2ab4e:	4501                	li	a0,0
  a2ab50:	8044                	popret	{ra,s0-s2},16
  a2ab52:	80000537          	lui	a0,0x80000
  a2ab56:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2ab58:	bfe5                	j	a2ab50 <hal_pmp_riscv31_config+0x42>

00a2ab5a <hal_pmp_riscv31_funcs_get>:
  a2ab5a:	00a3 3e04 051f      	l.li	a0,0xa33e04
  a2ab60:	8082                	ret

00a2ab62 <hal_pmp_riscv31_regs_set_pmpaddr>:
  a2ab62:	e501                	bnez	a0,a2ab6a <hal_pmp_riscv31_regs_set_pmpaddr+0x8>
  a2ab64:	3b059073          	csrw	pmpaddr0,a1
  a2ab68:	8082                	ret
  a2ab6a:	010512bb          	bnei	a0,1,a2ab74 <hal_pmp_riscv31_regs_set_pmpaddr+0x12>
  a2ab6e:	3b159073          	csrw	pmpaddr1,a1
  a2ab72:	8082                	ret
  a2ab74:	020512bb          	bnei	a0,2,a2ab7e <hal_pmp_riscv31_regs_set_pmpaddr+0x1c>
  a2ab78:	3b259073          	csrw	pmpaddr2,a1
  a2ab7c:	8082                	ret
  a2ab7e:	030512bb          	bnei	a0,3,a2ab88 <hal_pmp_riscv31_regs_set_pmpaddr+0x26>
  a2ab82:	3b359073          	csrw	pmpaddr3,a1
  a2ab86:	8082                	ret
  a2ab88:	040512bb          	bnei	a0,4,a2ab92 <hal_pmp_riscv31_regs_set_pmpaddr+0x30>
  a2ab8c:	3b459073          	csrw	pmpaddr4,a1
  a2ab90:	8082                	ret
  a2ab92:	050512bb          	bnei	a0,5,a2ab9c <hal_pmp_riscv31_regs_set_pmpaddr+0x3a>
  a2ab96:	3b559073          	csrw	pmpaddr5,a1
  a2ab9a:	8082                	ret
  a2ab9c:	060512bb          	bnei	a0,6,a2aba6 <hal_pmp_riscv31_regs_set_pmpaddr+0x44>
  a2aba0:	3b659073          	csrw	pmpaddr6,a1
  a2aba4:	8082                	ret
  a2aba6:	070512bb          	bnei	a0,7,a2abb0 <hal_pmp_riscv31_regs_set_pmpaddr+0x4e>
  a2abaa:	3b759073          	csrw	pmpaddr7,a1
  a2abae:	8082                	ret
  a2abb0:	080512bb          	bnei	a0,8,a2abba <hal_pmp_riscv31_regs_set_pmpaddr+0x58>
  a2abb4:	3b859073          	csrw	pmpaddr8,a1
  a2abb8:	8082                	ret
  a2abba:	090512bb          	bnei	a0,9,a2abc4 <hal_pmp_riscv31_regs_set_pmpaddr+0x62>
  a2abbe:	3b959073          	csrw	pmpaddr9,a1
  a2abc2:	8082                	ret
  a2abc4:	0a0512bb          	bnei	a0,10,a2abce <hal_pmp_riscv31_regs_set_pmpaddr+0x6c>
  a2abc8:	3ba59073          	csrw	pmpaddr10,a1
  a2abcc:	8082                	ret
  a2abce:	0b0512bb          	bnei	a0,11,a2abd8 <hal_pmp_riscv31_regs_set_pmpaddr+0x76>
  a2abd2:	3bb59073          	csrw	pmpaddr11,a1
  a2abd6:	8082                	ret
  a2abd8:	0c0512bb          	bnei	a0,12,a2abe2 <hal_pmp_riscv31_regs_set_pmpaddr+0x80>
  a2abdc:	3bc59073          	csrw	pmpaddr12,a1
  a2abe0:	8082                	ret
  a2abe2:	0d0512bb          	bnei	a0,13,a2abec <hal_pmp_riscv31_regs_set_pmpaddr+0x8a>
  a2abe6:	3bd59073          	csrw	pmpaddr13,a1
  a2abea:	8082                	ret
  a2abec:	0e0512bb          	bnei	a0,14,a2abf6 <hal_pmp_riscv31_regs_set_pmpaddr+0x94>
  a2abf0:	3be59073          	csrw	pmpaddr14,a1
  a2abf4:	8082                	ret
  a2abf6:	0f05123b          	bnei	a0,15,a2abfe <hal_pmp_riscv31_regs_set_pmpaddr+0x9c>
  a2abfa:	3bf59073          	csrw	pmpaddr15,a1
  a2abfe:	8082                	ret

00a2ac00 <hal_pmp_riscv31_regs_set_memxattr>:
  a2ac00:	0ff57713          	andi	a4,a0,255
  a2ac04:	080579bb          	bgeui	a0,8,a2ac2a <hal_pmp_riscv31_regs_set_memxattr+0x2a>
  a2ac08:	7d8027f3          	csrr	a5,0x7d8
  a2ac0c:	070a                	slli	a4,a4,0x2
  a2ac0e:	9f01                	uxtb	a4
  a2ac10:	46bd                	li	a3,15
  a2ac12:	00e696b3          	sll	a3,a3,a4
  a2ac16:	fff6c693          	not	a3,a3
  a2ac1a:	8efd                	and	a3,a3,a5
  a2ac1c:	00e59733          	sll	a4,a1,a4
  a2ac20:	8f55                	or	a4,a4,a3
  a2ac22:	82ba                	mv	t0,a4
  a2ac24:	7d829073          	csrw	0x7d8,t0
  a2ac28:	8082                	ret
  a2ac2a:	7d902673          	csrr	a2,0x7d9
  a2ac2e:	ff870793          	addi	a5,a4,-8 # 4410cff8 <_gp_+0x436d883c>
  a2ac32:	078a                	slli	a5,a5,0x2
  a2ac34:	9f81                	uxtb	a5
  a2ac36:	46bd                	li	a3,15
  a2ac38:	00f696b3          	sll	a3,a3,a5
  a2ac3c:	fff6c693          	not	a3,a3
  a2ac40:	8ef1                	and	a3,a3,a2
  a2ac42:	00f597b3          	sll	a5,a1,a5
  a2ac46:	8fd5                	or	a5,a5,a3
  a2ac48:	82be                	mv	t0,a5
  a2ac4a:	7d929073          	csrw	0x7d9,t0
  a2ac4e:	8082                	ret

00a2ac50 <hal_pmp_riscv31_regs_set_pmpxcfg>:
  a2ac50:	00255793          	srli	a5,a0,0x2
  a2ac54:	1141                	addi	sp,sp,-16
  a2ac56:	01078a3b          	beqi	a5,1,a2ac7e <hal_pmp_riscv31_regs_set_pmpxcfg+0x2e>
  a2ac5a:	c799                	beqz	a5,a2ac68 <hal_pmp_riscv31_regs_set_pmpxcfg+0x18>
  a2ac5c:	02078ebb          	beqi	a5,2,a2ac96 <hal_pmp_riscv31_regs_set_pmpxcfg+0x46>
  a2ac60:	031783bb          	beqi	a5,3,a2acae <hal_pmp_riscv31_regs_set_pmpxcfg+0x5e>
  a2ac64:	0141                	addi	sp,sp,16
  a2ac66:	8082                	ret
  a2ac68:	3a0027f3          	csrr	a5,pmpcfg0
  a2ac6c:	c63e                	sw	a5,12(sp)
  a2ac6e:	081c                	addi	a5,sp,16
  a2ac70:	953e                	add	a0,a0,a5
  a2ac72:	feb50e23          	sb	a1,-4(a0)
  a2ac76:	47b2                	lw	a5,12(sp)
  a2ac78:	3a079073          	csrw	pmpcfg0,a5
  a2ac7c:	b7e5                	j	a2ac64 <hal_pmp_riscv31_regs_set_pmpxcfg+0x14>
  a2ac7e:	3a1027f3          	csrr	a5,pmpcfg1
  a2ac82:	c63e                	sw	a5,12(sp)
  a2ac84:	890d                	andi	a0,a0,3
  a2ac86:	081c                	addi	a5,sp,16
  a2ac88:	953e                	add	a0,a0,a5
  a2ac8a:	feb50e23          	sb	a1,-4(a0)
  a2ac8e:	47b2                	lw	a5,12(sp)
  a2ac90:	3a179073          	csrw	pmpcfg1,a5
  a2ac94:	bfc1                	j	a2ac64 <hal_pmp_riscv31_regs_set_pmpxcfg+0x14>
  a2ac96:	3a2027f3          	csrr	a5,pmpcfg2
  a2ac9a:	c63e                	sw	a5,12(sp)
  a2ac9c:	890d                	andi	a0,a0,3
  a2ac9e:	081c                	addi	a5,sp,16
  a2aca0:	953e                	add	a0,a0,a5
  a2aca2:	feb50e23          	sb	a1,-4(a0)
  a2aca6:	47b2                	lw	a5,12(sp)
  a2aca8:	3a279073          	csrw	pmpcfg2,a5
  a2acac:	bf65                	j	a2ac64 <hal_pmp_riscv31_regs_set_pmpxcfg+0x14>
  a2acae:	3a3027f3          	csrr	a5,pmpcfg3
  a2acb2:	c63e                	sw	a5,12(sp)
  a2acb4:	890d                	andi	a0,a0,3
  a2acb6:	081c                	addi	a5,sp,16
  a2acb8:	953e                	add	a0,a0,a5
  a2acba:	feb50e23          	sb	a1,-4(a0)
  a2acbe:	47b2                	lw	a5,12(sp)
  a2acc0:	3a379073          	csrw	pmpcfg3,a5
  a2acc4:	b745                	j	a2ac64 <hal_pmp_riscv31_regs_set_pmpxcfg+0x14>

00a2acc6 <hal_reboot_chip>:
  a2acc6:	8018                	push	{ra},-16
  a2acc8:	c0dfe0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2accc:	591020ef          	jal	ra,a2da5c <set_cpu_utils_system_boot_magic>
  a2acd0:	461010ef          	jal	ra,a2c930 <reboot_port_reboot_chip>
  a2acd4:	a001                	j	a2acd4 <hal_reboot_chip+0xe>

00a2acd6 <hal_rkp_lock>:
  a2acd6:	8048                	push	{ra,s0-s2},-16
  a2acd8:	88aff0ef          	jal	ra,a29d62 <crypto_get_cpu_type>
  a2acdc:	c509                	beqz	a0,a2ace6 <hal_rkp_lock+0x10>
  a2acde:	01050cbb          	beqi	a0,1,a2ad10 <hal_rkp_lock+0x3a>
  a2ace2:	557d                	li	a0,-1
  a2ace4:	8044                	popret	{ra,s0-s2},16
  a2ace6:	4489                	li	s1,2
  a2ace8:	000f 4240 041f      	l.li	s0,0xf4240
  a2acee:	44112937          	lui	s2,0x44112
  a2acf2:	00992023          	sw	s1,0(s2) # 44112000 <_gp_+0x436dd844>
  a2acf6:	0ff0000f          	fence
  a2acfa:	00092783          	lw	a5,0(s2)
  a2acfe:	8b9d                	andi	a5,a5,7
  a2ad00:	00978a63          	beq	a5,s1,a2ad14 <hal_rkp_lock+0x3e>
  a2ad04:	4505                	li	a0,1
  a2ad06:	147d                	addi	s0,s0,-1
  a2ad08:	d8cff0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2ad0c:	f07d                	bnez	s0,a2acf2 <hal_rkp_lock+0x1c>
  a2ad0e:	bfd1                	j	a2ace2 <hal_rkp_lock+0xc>
  a2ad10:	4485                	li	s1,1
  a2ad12:	bfd9                	j	a2ace8 <hal_rkp_lock+0x12>
  a2ad14:	4501                	li	a0,0
  a2ad16:	b7f9                	j	a2ace4 <hal_rkp_lock+0xe>

00a2ad18 <hal_rkp_unlock>:
  a2ad18:	441127b7          	lui	a5,0x44112
  a2ad1c:	0007a023          	sw	zero,0(a5) # 44112000 <_gp_+0x436dd844>
  a2ad20:	0ff0000f          	fence
  a2ad24:	4501                	li	a0,0
  a2ad26:	8082                	ret

00a2ad28 <hal_rkp_kdf_wait_done>:
  a2ad28:	8138                	push	{ra,s0-s1},-32
  a2ad2a:	57fd                	li	a5,-1
  a2ad2c:	c63e                	sw	a5,12(sp)
  a2ad2e:	000f 4240 041f      	l.li	s0,0xf4240
  a2ad34:	441124b7          	lui	s1,0x44112
  a2ad38:	40dc                	lw	a5,4(s1)
  a2ad3a:	8b85                	andi	a5,a5,1
  a2ad3c:	ef91                	bnez	a5,a2ad58 <hal_rkp_kdf_wait_done+0x30>
  a2ad3e:	4785                	li	a5,1
  a2ad40:	c89c                	sw	a5,16(s1)
  a2ad42:	0ff0000f          	fence
  a2ad46:	c602                	sw	zero,12(sp)
  a2ad48:	441127b7          	lui	a5,0x44112
  a2ad4c:	479c                	lw	a5,8(a5)
  a2ad4e:	c399                	beqz	a5,a2ad54 <hal_rkp_kdf_wait_done+0x2c>
  a2ad50:	57fd                	li	a5,-1
  a2ad52:	c63e                	sw	a5,12(sp)
  a2ad54:	4532                	lw	a0,12(sp)
  a2ad56:	8134                	popret	{ra,s0-s1},32
  a2ad58:	4505                	li	a0,1
  a2ad5a:	147d                	addi	s0,s0,-1
  a2ad5c:	d38ff0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2ad60:	fc61                	bnez	s0,a2ad38 <hal_rkp_kdf_wait_done+0x10>
  a2ad62:	57fd                	li	a5,-1
  a2ad64:	c63e                	sw	a5,12(sp)
  a2ad66:	b7cd                	j	a2ad48 <hal_rkp_kdf_wait_done+0x20>

00a2ad68 <hal_rkp_kdf_hard_calculation>:
  a2ad68:	8238                	push	{ra,s0-s1},-48
  a2ad6a:	57fd                	li	a5,-1
  a2ad6c:	c63e                	sw	a5,12(sp)
  a2ad6e:	c802                	sw	zero,16(sp)
  a2ad70:	ca02                	sw	zero,20(sp)
  a2ad72:	cc02                	sw	zero,24(sp)
  a2ad74:	ce02                	sw	zero,28(sp)
  a2ad76:	0e050863          	beqz	a0,a2ae66 <hal_rkp_kdf_hard_calculation+0xfe>
  a2ad7a:	4504                	lw	s1,8(a0)
  a2ad7c:	c485                	beqz	s1,a2ada4 <hal_rkp_kdf_hard_calculation+0x3c>
  a2ad7e:	4558                	lw	a4,12(a0)
  a2ad80:	1c07193b          	bnei	a4,28,a2ada4 <hal_rkp_kdf_hard_calculation+0x3c>
  a2ad84:	2158                	lbu	a4,4(a0)
  a2ad86:	020777bb          	bgeui	a4,2,a2ada4 <hal_rkp_kdf_hard_calculation+0x3c>
  a2ad8a:	4110                	lw	a2,0(a0)
  a2ad8c:	842a                	mv	s0,a0
  a2ad8e:	0814                	addi	a3,sp,16
  a2ad90:	458d                	li	a1,3
  a2ad92:	00a3 3e30 051f      	l.li	a0,0xa33e30
  a2ad98:	fe3fe0ef          	jal	ra,a29d7a <crypto_get_value_by_index>
  a2ad9c:	c62a                	sw	a0,12(sp)
  a2ad9e:	47b2                	lw	a5,12(sp)
  a2ada0:	c781                	beqz	a5,a2ada8 <hal_rkp_kdf_hard_calculation+0x40>
  a2ada2:	47b2                	lw	a5,12(sp)
  a2ada4:	853e                	mv	a0,a5
  a2ada6:	8234                	popret	{ra,s0-s1},48
  a2ada8:	4010                	lw	a2,0(s0)
  a2adaa:	0874                	addi	a3,sp,28
  a2adac:	458d                	li	a1,3
  a2adae:	00a3 3e48 051f      	l.li	a0,0xa33e48
  a2adb4:	fc7fe0ef          	jal	ra,a29d7a <crypto_get_value_by_index>
  a2adb8:	c62a                	sw	a0,12(sp)
  a2adba:	47b2                	lw	a5,12(sp)
  a2adbc:	f3fd                	bnez	a5,a2ada2 <hal_rkp_kdf_hard_calculation+0x3a>
  a2adbe:	2050                	lbu	a2,4(s0)
  a2adc0:	0854                	addi	a3,sp,20
  a2adc2:	4589                	li	a1,2
  a2adc4:	00a3 3e08 051f      	l.li	a0,0xa33e08
  a2adca:	fb1fe0ef          	jal	ra,a29d7a <crypto_get_value_by_index>
  a2adce:	c62a                	sw	a0,12(sp)
  a2add0:	47b2                	lw	a5,12(sp)
  a2add2:	fbe1                	bnez	a5,a2ada2 <hal_rkp_kdf_hard_calculation+0x3a>
  a2add4:	3050                	lbu	a2,5(s0)
  a2add6:	0834                	addi	a3,sp,24
  a2add8:	458d                	li	a1,3
  a2adda:	00a3 3e18 051f      	l.li	a0,0xa33e18
  a2ade0:	f9bfe0ef          	jal	ra,a29d7a <crypto_get_value_by_index>
  a2ade4:	c62a                	sw	a0,12(sp)
  a2ade6:	47b2                	lw	a5,12(sp)
  a2ade8:	ffcd                	bnez	a5,a2ada2 <hal_rkp_kdf_hard_calculation+0x3a>
  a2adea:	44112737          	lui	a4,0x44112
  a2adee:	281c                	lbu	a5,16(s0)
  a2adf0:	4672                	lw	a2,28(sp)
  a2adf2:	36072683          	lw	a3,864(a4) # 44112360 <_gp_+0x436ddba4>
  a2adf6:	00c797b3          	sll	a5,a5,a2
  a2adfa:	8fd5                	or	a5,a5,a3
  a2adfc:	36f72023          	sw	a5,864(a4)
  a2ae00:	0ff0000f          	fence
  a2ae04:	4701                	li	a4,0
  a2ae06:	87a6                	mv	a5,s1
  a2ae08:	1104 48d0 061f      	l.li	a2,0x110448d0
  a2ae0e:	4454                	lw	a3,12(s0)
  a2ae10:	8289                	srli	a3,a3,0x2
  a2ae12:	04d76063          	bltu	a4,a3,a2ae52 <hal_rkp_kdf_hard_calculation+0xea>
  a2ae16:	44112637          	lui	a2,0x44112
  a2ae1a:	425c                	lw	a5,4(a2)
  a2ae1c:	4762                	lw	a4,24(sp)
  a2ae1e:	76d1                	lui	a3,0xffff4
  a2ae20:	16fd                	addi	a3,a3,-1 # ffff3fff <_gp_+0xff5bf843>
  a2ae22:	8b0d                	andi	a4,a4,3
  a2ae24:	8ff5                	and	a5,a5,a3
  a2ae26:	1ce7a79b          	orshf	a5,a5,a4,sll,14
  a2ae2a:	4752                	lw	a4,20(sp)
  a2ae2c:	9bc5                	andi	a5,a5,-15
  a2ae2e:	8b1d                	andi	a4,a4,7
  a2ae30:	02e7a79b          	orshf	a5,a5,a4,sll,1
  a2ae34:	4742                	lw	a4,16(sp)
  a2ae36:	e0f7f793          	andi	a5,a5,-497
  a2ae3a:	8b7d                	andi	a4,a4,31
  a2ae3c:	08e7a79b          	orshf	a5,a5,a4,sll,4
  a2ae40:	0017e793          	ori	a5,a5,1
  a2ae44:	c25c                	sw	a5,4(a2)
  a2ae46:	0ff0000f          	fence
  a2ae4a:	3df9                	jal	ra,a2ad28 <hal_rkp_kdf_wait_done>
  a2ae4c:	c62a                	sw	a0,12(sp)
  a2ae4e:	47b2                	lw	a5,12(sp)
  a2ae50:	bf89                	j	a2ada2 <hal_rkp_kdf_hard_calculation+0x3a>
  a2ae52:	438c                	lw	a1,0(a5)
  a2ae54:	00c706b3          	add	a3,a4,a2
  a2ae58:	068a                	slli	a3,a3,0x2
  a2ae5a:	c28c                	sw	a1,0(a3)
  a2ae5c:	0ff0000f          	fence
  a2ae60:	0705                	addi	a4,a4,1
  a2ae62:	0791                	addi	a5,a5,4 # 44112004 <_gp_+0x436dd848>
  a2ae64:	b76d                	j	a2ae0e <hal_rkp_kdf_hard_calculation+0xa6>
  a2ae66:	57fd                	li	a5,-1
  a2ae68:	bf35                	j	a2ada4 <hal_rkp_kdf_hard_calculation+0x3c>

00a2ae6a <inner_kslot_chn_is_locked>:
  a2ae6a:	44114737          	lui	a4,0x44114
  a2ae6e:	b3072783          	lw	a5,-1232(a4) # 44113b30 <_gp_+0x436df374>
  a2ae72:	76e1                	lui	a3,0xffff8
  a2ae74:	16fd                	addi	a3,a3,-1 # ffff7fff <_gp_+0xff5c3843>
  a2ae76:	8985                	andi	a1,a1,1
  a2ae78:	8ff5                	and	a5,a5,a3
  a2ae7a:	1eb7a79b          	orshf	a5,a5,a1,sll,15
  a2ae7e:	3ff57513          	andi	a0,a0,1023
  a2ae82:	c007f793          	andi	a5,a5,-1024
  a2ae86:	8fc9                	or	a5,a5,a0
  a2ae88:	b2f72823          	sw	a5,-1232(a4)
  a2ae8c:	0ff0000f          	fence
  a2ae90:	b3472503          	lw	a0,-1228(a4)
  a2ae94:	891d                	andi	a0,a0,7
  a2ae96:	c211                	beqz	a2,a2ae9a <inner_kslot_chn_is_locked+0x30>
  a2ae98:	c208                	sw	a0,0(a2)
  a2ae9a:	00a03533          	snez	a0,a0
  a2ae9e:	8082                	ret

00a2aea0 <inner_chk_kslot_busy>:
  a2aea0:	8028                	push	{ra,s0},-16
  a2aea2:	842a                	mv	s0,a0
  a2aea4:	ebffe0ef          	jal	ra,a29d62 <crypto_get_cpu_type>
  a2aea8:	c911                	beqz	a0,a2aebc <inner_chk_kslot_busy+0x1c>
  a2aeaa:	0105173b          	bnei	a0,1,a2aec6 <inner_chk_kslot_busy+0x26>
  a2aeae:	441147b7          	lui	a5,0x44114
  a2aeb2:	b147a783          	lw	a5,-1260(a5) # 44113b14 <_gp_+0x436df358>
  a2aeb6:	c01c                	sw	a5,0(s0)
  a2aeb8:	4501                	li	a0,0
  a2aeba:	8024                	popret	{ra,s0},16
  a2aebc:	441147b7          	lui	a5,0x44114
  a2aec0:	b107a783          	lw	a5,-1264(a5) # 44113b10 <_gp_+0x436df354>
  a2aec4:	bfcd                	j	a2aeb6 <inner_chk_kslot_busy+0x16>
  a2aec6:	4505                	li	a0,1
  a2aec8:	bfcd                	j	a2aeba <inner_chk_kslot_busy+0x1a>

00a2aeca <inner_kslot_is_busy>:
  a2aeca:	8118                	push	{ra},-32
  a2aecc:	0068                	addi	a0,sp,12
  a2aece:	c602                	sw	zero,12(sp)
  a2aed0:	3fc1                	jal	ra,a2aea0 <inner_chk_kslot_busy>
  a2aed2:	4785                	li	a5,1
  a2aed4:	e119                	bnez	a0,a2aeda <inner_kslot_is_busy+0x10>
  a2aed6:	47b2                	lw	a5,12(sp)
  a2aed8:	8b85                	andi	a5,a5,1
  a2aeda:	853e                	mv	a0,a5
  a2aedc:	8114                	popret	{ra},32

00a2aede <hal_keyslot_lock>:
  a2aede:	8248                	push	{ra,s0-s2},-48
  a2aee0:	57fd                	li	a5,-1
  a2aee2:	c62e                	sw	a1,12(sp)
  a2aee4:	84aa                	mv	s1,a0
  a2aee6:	cc3e                	sw	a5,24(sp)
  a2aee8:	e7bfe0ef          	jal	ra,a29d62 <crypto_get_cpu_type>
  a2aeec:	45b2                	lw	a1,12(sp)
  a2aeee:	ce02                	sw	zero,28(sp)
  a2aef0:	892a                	mv	s2,a0
  a2aef2:	c591                	beqz	a1,a2aefe <hal_keyslot_lock+0x20>
  a2aef4:	0105863b          	beqi	a1,1,a2af0c <hal_keyslot_lock+0x2e>
  a2aef8:	4401                	li	s0,0
  a2aefa:	57fd                	li	a5,-1
  a2aefc:	a019                	j	a2af02 <hal_keyslot_lock+0x24>
  a2aefe:	4401                	li	s0,0
  a2af00:	4781                	li	a5,0
  a2af02:	cc3e                	sw	a5,24(sp)
  a2af04:	47e2                	lw	a5,24(sp)
  a2af06:	c789                	beqz	a5,a2af10 <hal_keyslot_lock+0x32>
  a2af08:	4562                	lw	a0,24(sp)
  a2af0a:	8244                	popret	{ra,s0-s2},48
  a2af0c:	4405                	li	s0,1
  a2af0e:	bfcd                	j	a2af00 <hal_keyslot_lock+0x22>
  a2af10:	4601                	li	a2,0
  a2af12:	85a2                	mv	a1,s0
  a2af14:	8526                	mv	a0,s1
  a2af16:	3f91                	jal	ra,a2ae6a <inner_kslot_chn_is_locked>
  a2af18:	c119                	beqz	a0,a2af1e <hal_keyslot_lock+0x40>
  a2af1a:	557d                	li	a0,-1
  a2af1c:	b7fd                	j	a2af0a <hal_keyslot_lock+0x2c>
  a2af1e:	3775                	jal	ra,a2aeca <inner_kslot_is_busy>
  a2af20:	fd6d                	bnez	a0,a2af1a <hal_keyslot_lock+0x3c>
  a2af22:	04091763          	bnez	s2,a2af70 <hal_keyslot_lock+0x92>
  a2af26:	4909                	li	s2,2
  a2af28:	0000 1b00 071f      	l.li	a4,0x1b00
  a2af2e:	441127b7          	lui	a5,0x44112
  a2af32:	973e                	add	a4,a4,a5
  a2af34:	431c                	lw	a5,0(a4)
  a2af36:	76e1                	lui	a3,0xffff8
  a2af38:	16fd                	addi	a3,a3,-1 # ffff7fff <_gp_+0xff5c3843>
  a2af3a:	8ff5                	and	a5,a5,a3
  a2af3c:	1e87a79b          	orshf	a5,a5,s0,sll,15
  a2af40:	3ff4f693          	andi	a3,s1,1023
  a2af44:	c007f793          	andi	a5,a5,-1024
  a2af48:	8fd5                	or	a5,a5,a3
  a2af4a:	001006b7          	lui	a3,0x100
  a2af4e:	8fd5                	or	a5,a5,a3
  a2af50:	c31c                	sw	a5,0(a4)
  a2af52:	0ff0000f          	fence
  a2af56:	0870                	addi	a2,sp,28
  a2af58:	85a2                	mv	a1,s0
  a2af5a:	8526                	mv	a0,s1
  a2af5c:	3739                	jal	ra,a2ae6a <inner_kslot_chn_is_locked>
  a2af5e:	dd55                	beqz	a0,a2af1a <hal_keyslot_lock+0x3c>
  a2af60:	4572                	lw	a0,28(sp)
  a2af62:	41250533          	sub	a0,a0,s2
  a2af66:	00a03533          	snez	a0,a0
  a2af6a:	40a00533          	neg	a0,a0
  a2af6e:	bf71                	j	a2af0a <hal_keyslot_lock+0x2c>
  a2af70:	4905                	li	s2,1
  a2af72:	0000 1b04 071f      	l.li	a4,0x1b04
  a2af78:	bf5d                	j	a2af2e <hal_keyslot_lock+0x50>

00a2af7a <hal_keyslot_unlock>:
  a2af7a:	8248                	push	{ra,s0-s2},-48
  a2af7c:	57fd                	li	a5,-1
  a2af7e:	c62e                	sw	a1,12(sp)
  a2af80:	842a                	mv	s0,a0
  a2af82:	cc3e                	sw	a5,24(sp)
  a2af84:	ddffe0ef          	jal	ra,a29d62 <crypto_get_cpu_type>
  a2af88:	45b2                	lw	a1,12(sp)
  a2af8a:	892a                	mv	s2,a0
  a2af8c:	c591                	beqz	a1,a2af98 <hal_keyslot_unlock+0x1e>
  a2af8e:	010586bb          	beqi	a1,1,a2afa8 <hal_keyslot_unlock+0x2e>
  a2af92:	4481                	li	s1,0
  a2af94:	57fd                	li	a5,-1
  a2af96:	a019                	j	a2af9c <hal_keyslot_unlock+0x22>
  a2af98:	4481                	li	s1,0
  a2af9a:	4781                	li	a5,0
  a2af9c:	cc3e                	sw	a5,24(sp)
  a2af9e:	47e2                	lw	a5,24(sp)
  a2afa0:	c791                	beqz	a5,a2afac <hal_keyslot_unlock+0x32>
  a2afa2:	47e2                	lw	a5,24(sp)
  a2afa4:	853e                	mv	a0,a5
  a2afa6:	8244                	popret	{ra,s0-s2},48
  a2afa8:	4485                	li	s1,1
  a2afaa:	bfc5                	j	a2af9a <hal_keyslot_unlock+0x20>
  a2afac:	3f39                	jal	ra,a2aeca <inner_kslot_is_busy>
  a2afae:	57fd                	li	a5,-1
  a2afb0:	f975                	bnez	a0,a2afa4 <hal_keyslot_unlock+0x2a>
  a2afb2:	4601                	li	a2,0
  a2afb4:	85a6                	mv	a1,s1
  a2afb6:	8522                	mv	a0,s0
  a2afb8:	3d4d                	jal	ra,a2ae6a <inner_kslot_chn_is_locked>
  a2afba:	4781                	li	a5,0
  a2afbc:	d565                	beqz	a0,a2afa4 <hal_keyslot_unlock+0x2a>
  a2afbe:	06090363          	beqz	s2,a2b024 <hal_keyslot_unlock+0xaa>
  a2afc2:	011917bb          	bnei	s2,1,a2b020 <hal_keyslot_unlock+0xa6>
  a2afc6:	0000 1b04 079f      	l.li	a5,0x1b04
  a2afcc:	44112737          	lui	a4,0x44112
  a2afd0:	97ba                	add	a5,a5,a4
  a2afd2:	4388                	lw	a0,0(a5)
  a2afd4:	7761                	lui	a4,0xffff8
  a2afd6:	177d                	addi	a4,a4,-1 # ffff7fff <_gp_+0xff5c3843>
  a2afd8:	8d79                	and	a0,a0,a4
  a2afda:	1e95251b          	orshf	a0,a0,s1,sll,15
  a2afde:	3ff47413          	andi	s0,s0,1023
  a2afe2:	c0057513          	andi	a0,a0,-1024
  a2afe6:	8d41                	or	a0,a0,s0
  a2afe8:	ffef ffff 071f      	l.li	a4,0xffefffff
  a2afee:	8d79                	and	a0,a0,a4
  a2aff0:	c388                	sw	a0,0(a5)
  a2aff2:	0ff0000f          	fence
  a2aff6:	000f 4240 041f      	l.li	s0,0xf4240
  a2affc:	35f9                	jal	ra,a2aeca <inner_kslot_is_busy>
  a2affe:	c51d                	beqz	a0,a2b02c <hal_keyslot_unlock+0xb2>
  a2b000:	4505                	li	a0,1
  a2b002:	147d                	addi	s0,s0,-1
  a2b004:	a90ff0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2b008:	f875                	bnez	s0,a2affc <hal_keyslot_unlock+0x82>
  a2b00a:	57fd                	li	a5,-1
  a2b00c:	cc3e                	sw	a5,24(sp)
  a2b00e:	47e2                	lw	a5,24(sp)
  a2b010:	fbc9                	bnez	a5,a2afa2 <hal_keyslot_unlock+0x28>
  a2b012:	0868                	addi	a0,sp,28
  a2b014:	ce02                	sw	zero,28(sp)
  a2b016:	3569                	jal	ra,a2aea0 <inner_chk_kslot_busy>
  a2b018:	e501                	bnez	a0,a2b020 <hal_keyslot_unlock+0xa6>
  a2b01a:	47f2                	lw	a5,28(sp)
  a2b01c:	8b89                	andi	a5,a5,2
  a2b01e:	d3d9                	beqz	a5,a2afa4 <hal_keyslot_unlock+0x2a>
  a2b020:	57fd                	li	a5,-1
  a2b022:	b749                	j	a2afa4 <hal_keyslot_unlock+0x2a>
  a2b024:	0000 1b00 079f      	l.li	a5,0x1b00
  a2b02a:	b74d                	j	a2afcc <hal_keyslot_unlock+0x52>
  a2b02c:	4781                	li	a5,0
  a2b02e:	bff9                	j	a2b00c <hal_keyslot_unlock+0x92>

00a2b030 <inner_klad_set_key_odd>:
  a2b030:	441137b7          	lui	a5,0x44113
  a2b034:	4b98                	lw	a4,16(a5)
  a2b036:	ffe77793          	andi	a5,a4,-2
  a2b03a:	c119                	beqz	a0,a2b040 <inner_klad_set_key_odd+0x10>
  a2b03c:	00176793          	ori	a5,a4,1
  a2b040:	44113737          	lui	a4,0x44113
  a2b044:	cb1c                	sw	a5,16(a4)
  a2b046:	0ff0000f          	fence
  a2b04a:	8082                	ret

00a2b04c <inner_klad_wait_com_route_done>:
  a2b04c:	8138                	push	{ra,s0-s1},-32
  a2b04e:	57fd                	li	a5,-1
  a2b050:	c23e                	sw	a5,4(sp)
  a2b052:	000f 4240 041f      	l.li	s0,0xf4240
  a2b058:	441134b7          	lui	s1,0x44113
  a2b05c:	0844a783          	lw	a5,132(s1) # 44113084 <_gp_+0x436de8c8>
  a2b060:	8b85                	andi	a5,a5,1
  a2b062:	eb85                	bnez	a5,a2b092 <inner_klad_wait_com_route_done+0x46>
  a2b064:	40fc                	lw	a5,68(s1)
  a2b066:	0017e793          	ori	a5,a5,1
  a2b06a:	c43e                	sw	a5,8(sp)
  a2b06c:	c0fc                	sw	a5,68(s1)
  a2b06e:	0ff0000f          	fence
  a2b072:	c202                	sw	zero,4(sp)
  a2b074:	44113737          	lui	a4,0x44113
  a2b078:	08872783          	lw	a5,136(a4) # 44113088 <_gp_+0x436de8cc>
  a2b07c:	5f1c                	lw	a5,56(a4)
  a2b07e:	5f58                	lw	a4,60(a4)
  a2b080:	8fd9                	or	a5,a5,a4
  a2b082:	00f037b3          	snez	a5,a5
  a2b086:	40f007b3          	neg	a5,a5
  a2b08a:	c23e                	sw	a5,4(sp)
  a2b08c:	4792                	lw	a5,4(sp)
  a2b08e:	4512                	lw	a0,4(sp)
  a2b090:	8134                	popret	{ra,s0-s1},32
  a2b092:	4505                	li	a0,1
  a2b094:	147d                	addi	s0,s0,-1
  a2b096:	9feff0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2b09a:	f069                	bnez	s0,a2b05c <inner_klad_wait_com_route_done+0x10>
  a2b09c:	bfe1                	j	a2b074 <inner_klad_wait_com_route_done+0x28>

00a2b09e <hal_klad_lock>:
  a2b09e:	8158                	push	{ra,s0-s3},-48
  a2b0a0:	57fd                	li	a5,-1
  a2b0a2:	c63e                	sw	a5,12(sp)
  a2b0a4:	0000 2710 041f      	l.li	s0,0x2710
  a2b0aa:	4905                	li	s2,1
  a2b0ac:	441134b7          	lui	s1,0x44113
  a2b0b0:	4991                	li	s3,4
  a2b0b2:	cb1fe0ef          	jal	ra,a29d62 <crypto_get_cpu_type>
  a2b0b6:	57fd                	li	a5,-1
  a2b0b8:	03251463          	bne	a0,s2,a2b0e0 <hal_klad_lock+0x42>
  a2b0bc:	0724aa23          	sw	s2,116(s1) # 44113074 <_gp_+0x436de8b8>
  a2b0c0:	0ff0000f          	fence
  a2b0c4:	5cf8                	lw	a4,124(s1)
  a2b0c6:	57fd                	li	a5,-1
  a2b0c8:	8b31                	andi	a4,a4,12
  a2b0ca:	01370b63          	beq	a4,s3,a2b0e0 <hal_klad_lock+0x42>
  a2b0ce:	0804a783          	lw	a5,128(s1)
  a2b0d2:	9f81                	uxtb	a5
  a2b0d4:	f5678793          	addi	a5,a5,-170 # 44112f56 <_gp_+0x436de79a>
  a2b0d8:	00f037b3          	snez	a5,a5
  a2b0dc:	40f007b3          	neg	a5,a5
  a2b0e0:	c63e                	sw	a5,12(sp)
  a2b0e2:	47b2                	lw	a5,12(sp)
  a2b0e4:	cb81                	beqz	a5,a2b0f4 <hal_klad_lock+0x56>
  a2b0e6:	4505                	li	a0,1
  a2b0e8:	147d                	addi	s0,s0,-1
  a2b0ea:	9aaff0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2b0ee:	f071                	bnez	s0,a2b0b2 <hal_klad_lock+0x14>
  a2b0f0:	557d                	li	a0,-1
  a2b0f2:	8154                	popret	{ra,s0-s3},48
  a2b0f4:	4501                	li	a0,0
  a2b0f6:	bff5                	j	a2b0f2 <hal_klad_lock+0x54>

00a2b0f8 <hal_klad_unlock>:
  a2b0f8:	8018                	push	{ra},-16
  a2b0fa:	c69fe0ef          	jal	ra,a29d62 <crypto_get_cpu_type>
  a2b0fe:	01051cbb          	bnei	a0,1,a2b130 <hal_klad_unlock+0x38>
  a2b102:	44113737          	lui	a4,0x44113
  a2b106:	08072683          	lw	a3,128(a4) # 44113080 <_gp_+0x436de8c4>
  a2b10a:	0aa00613          	li	a2,170
  a2b10e:	4781                	li	a5,0
  a2b110:	9e81                	uxtb	a3
  a2b112:	00c69d63          	bne	a3,a2,a2b12c <hal_klad_unlock+0x34>
  a2b116:	df28                	sw	a0,120(a4)
  a2b118:	0ff0000f          	fence
  a2b11c:	5f7c                	lw	a5,124(a4)
  a2b11e:	0307f793          	andi	a5,a5,48
  a2b122:	17c1                	addi	a5,a5,-16
  a2b124:	0017b793          	seqz	a5,a5
  a2b128:	40f007b3          	neg	a5,a5
  a2b12c:	853e                	mv	a0,a5
  a2b12e:	8014                	popret	{ra},16
  a2b130:	57fd                	li	a5,-1
  a2b132:	bfed                	j	a2b12c <hal_klad_unlock+0x34>

00a2b134 <hal_klad_set_key_crypto_cfg>:
  a2b134:	0a000793          	li	a5,160
  a2b138:	02f60563          	beq	a2,a5,a2b162 <hal_klad_set_key_crypto_cfg+0x2e>
  a2b13c:	00c7e963          	bltu	a5,a2,a2b14e <hal_klad_set_key_crypto_cfg+0x1a>
  a2b140:	05000693          	li	a3,80
  a2b144:	200615bb          	bnei	a2,32,a2b15a <hal_klad_set_key_crypto_cfg+0x26>
  a2b148:	02000693          	li	a3,32
  a2b14c:	a829                	j	a2b166 <hal_klad_set_key_crypto_cfg+0x32>
  a2b14e:	0a100793          	li	a5,161
  a2b152:	04f60363          	beq	a2,a5,a2b198 <hal_klad_set_key_crypto_cfg+0x64>
  a2b156:	0a200693          	li	a3,162
  a2b15a:	00d60663          	beq	a2,a3,a2b166 <hal_klad_set_key_crypto_cfg+0x32>
  a2b15e:	557d                	li	a0,-1
  a2b160:	8082                	ret
  a2b162:	0a000693          	li	a3,160
  a2b166:	44113637          	lui	a2,0x44113
  a2b16a:	4a58                	lw	a4,20(a2)
  a2b16c:	7781                	lui	a5,0xfffe0
  a2b16e:	17fd                	addi	a5,a5,-1 # fffdffff <_gp_+0xff5ab843>
  a2b170:	8ff9                	and	a5,a5,a4
  a2b172:	8985                	andi	a1,a1,1
  a2b174:	22b7a59b          	orshf	a1,a5,a1,sll,17
  a2b178:	77c1                	lui	a5,0xffff0
  a2b17a:	17fd                	addi	a5,a5,-1 # fffeffff <_gp_+0xff5bb843>
  a2b17c:	8905                	andi	a0,a0,1
  a2b17e:	8fed                	and	a5,a5,a1
  a2b180:	20a7a79b          	orshf	a5,a5,a0,sll,16
  a2b184:	757d                	lui	a0,0xfffff
  a2b186:	053d                	addi	a0,a0,15 # fffff00f <_gp_+0xff5ca853>
  a2b188:	8fe9                	and	a5,a5,a0
  a2b18a:	08d7a79b          	orshf	a5,a5,a3,sll,4
  a2b18e:	ca5c                	sw	a5,20(a2)
  a2b190:	0ff0000f          	fence
  a2b194:	4501                	li	a0,0
  a2b196:	8082                	ret
  a2b198:	0a100693          	li	a3,161
  a2b19c:	b7e9                	j	a2b166 <hal_klad_set_key_crypto_cfg+0x32>

00a2b19e <hal_klad_set_key_dest_cfg>:
  a2b19e:	0205633b          	bltui	a0,2,a2b1aa <hal_klad_set_key_dest_cfg+0xc>
  a2b1a2:	02050bbb          	beqi	a0,2,a2b1d0 <hal_klad_set_key_dest_cfg+0x32>
  a2b1a6:	557d                	li	a0,-1
  a2b1a8:	8082                	ret
  a2b1aa:	4705                	li	a4,1
  a2b1ac:	441137b7          	lui	a5,0x44113
  a2b1b0:	4bdc                	lw	a5,20(a5)
  a2b1b2:	9be1                	andi	a5,a5,-8
  a2b1b4:	8fd9                	or	a5,a5,a4
  a2b1b6:	0205133b          	bnei	a0,2,a2b1c2 <hal_klad_set_key_dest_cfg+0x24>
  a2b1ba:	fff3 ffff 071f      	l.li	a4,0xfff3ffff
  a2b1c0:	8ff9                	and	a5,a5,a4
  a2b1c2:	44113737          	lui	a4,0x44113
  a2b1c6:	cb5c                	sw	a5,20(a4)
  a2b1c8:	0ff0000f          	fence
  a2b1cc:	4501                	li	a0,0
  a2b1ce:	8082                	ret
  a2b1d0:	471d                	li	a4,7
  a2b1d2:	dde9                	beqz	a1,a2b1ac <hal_klad_set_key_dest_cfg+0xe>
  a2b1d4:	bfc9                	j	a2b1a6 <hal_klad_set_key_dest_cfg+0x8>

00a2b1d6 <hal_klad_set_key_secure_cfg>:
  a2b1d6:	c539                	beqz	a0,a2b224 <hal_klad_set_key_secure_cfg+0x4e>
  a2b1d8:	441136b7          	lui	a3,0x44113
  a2b1dc:	4e9c                	lw	a5,24(a3)
  a2b1de:	3118                	lbu	a4,1(a0)
  a2b1e0:	fdf7f793          	andi	a5,a5,-33
  a2b1e4:	8b05                	andi	a4,a4,1
  a2b1e6:	0ae7a79b          	orshf	a5,a5,a4,sll,5
  a2b1ea:	2138                	lbu	a4,2(a0)
  a2b1ec:	9bbd                	andi	a5,a5,-17
  a2b1ee:	8b05                	andi	a4,a4,1
  a2b1f0:	08e7a79b          	orshf	a5,a5,a4,sll,4
  a2b1f4:	3138                	lbu	a4,3(a0)
  a2b1f6:	9bdd                	andi	a5,a5,-9
  a2b1f8:	8b05                	andi	a4,a4,1
  a2b1fa:	06e7a79b          	orshf	a5,a5,a4,sll,3
  a2b1fe:	2158                	lbu	a4,4(a0)
  a2b200:	9bed                	andi	a5,a5,-5
  a2b202:	8b05                	andi	a4,a4,1
  a2b204:	04e7a79b          	orshf	a5,a5,a4,sll,2
  a2b208:	3158                	lbu	a4,5(a0)
  a2b20a:	9bf5                	andi	a5,a5,-3
  a2b20c:	8b05                	andi	a4,a4,1
  a2b20e:	02e7a79b          	orshf	a5,a5,a4,sll,1
  a2b212:	2118                	lbu	a4,0(a0)
  a2b214:	9bf9                	andi	a5,a5,-2
  a2b216:	8b05                	andi	a4,a4,1
  a2b218:	8fd9                	or	a5,a5,a4
  a2b21a:	ce9c                	sw	a5,24(a3)
  a2b21c:	0ff0000f          	fence
  a2b220:	4501                	li	a0,0
  a2b222:	8082                	ret
  a2b224:	557d                	li	a0,-1
  a2b226:	8082                	ret

00a2b228 <hal_klad_set_key_addr>:
  a2b228:	e105                	bnez	a0,a2b248 <hal_klad_set_key_addr+0x20>
  a2b22a:	44113737          	lui	a4,0x44113
  a2b22e:	4b1c                	lw	a5,16(a4)
  a2b230:	0586                	slli	a1,a1,0x1
  a2b232:	3fe5f593          	andi	a1,a1,1022
  a2b236:	c007f793          	andi	a5,a5,-1024
  a2b23a:	8ddd                	or	a1,a1,a5
  a2b23c:	cb0c                	sw	a1,16(a4)
  a2b23e:	0ff0000f          	fence
  a2b242:	4781                	li	a5,0
  a2b244:	853e                	mv	a0,a5
  a2b246:	8082                	ret
  a2b248:	57fd                	li	a5,-1
  a2b24a:	01f51ebb          	bnei	a0,1,a2b244 <hal_klad_set_key_addr+0x1c>
  a2b24e:	44113737          	lui	a4,0x44113
  a2b252:	4b1c                	lw	a5,16(a4)
  a2b254:	3ff5f593          	andi	a1,a1,1023
  a2b258:	bff9                	j	a2b236 <hal_klad_set_key_addr+0xe>

00a2b25a <hal_klad_start_com_route>:
  a2b25a:	8148                	push	{ra,s0-s2},-32
  a2b25c:	57fd                	li	a5,-1
  a2b25e:	c23e                	sw	a5,4(sp)
  a2b260:	c402                	sw	zero,8(sp)
  a2b262:	c602                	sw	zero,12(sp)
  a2b264:	cdc5                	beqz	a1,a2b31c <hal_klad_start_com_route+0xc2>
  a2b266:	41d8                	lw	a4,4(a1)
  a2b268:	cf19                	beqz	a4,a2b286 <hal_klad_start_com_route+0x2c>
  a2b26a:	8932                	mv	s2,a2
  a2b26c:	84ae                	mv	s1,a1
  a2b26e:	862a                	mv	a2,a0
  a2b270:	0074                	addi	a3,sp,12
  a2b272:	458d                	li	a1,3
  a2b274:	00a3 3e78 051f      	l.li	a0,0xa33e78
  a2b27a:	b01fe0ef          	jal	ra,a29d7a <crypto_get_value_by_index>
  a2b27e:	c22a                	sw	a0,4(sp)
  a2b280:	4792                	lw	a5,4(sp)
  a2b282:	c781                	beqz	a5,a2b28a <hal_klad_start_com_route+0x30>
  a2b284:	4792                	lw	a5,4(sp)
  a2b286:	853e                	mv	a0,a5
  a2b288:	8144                	popret	{ra,s0-s2},32
  a2b28a:	20b0                	lbu	a2,2(s1)
  a2b28c:	0034                	addi	a3,sp,8
  a2b28e:	458d                	li	a1,3
  a2b290:	00a3 3e60 051f      	l.li	a0,0xa33e60
  a2b296:	ae5fe0ef          	jal	ra,a29d7a <crypto_get_value_by_index>
  a2b29a:	c22a                	sw	a0,4(sp)
  a2b29c:	4792                	lw	a5,4(sp)
  a2b29e:	f3fd                	bnez	a5,a2b284 <hal_klad_start_com_route+0x2a>
  a2b2a0:	20bc                	lbu	a5,2(s1)
  a2b2a2:	4501                	li	a0,0
  a2b2a4:	e391                	bnez	a5,a2b2a8 <hal_klad_start_com_route+0x4e>
  a2b2a6:	3088                	lbu	a0,1(s1)
  a2b2a8:	00091463          	bnez	s2,a2b2b0 <hal_klad_start_com_route+0x56>
  a2b2ac:	d85ff0ef          	jal	ra,a2b030 <inner_klad_set_key_odd>
  a2b2b0:	4422                	lw	s0,8(sp)
  a2b2b2:	47b2                	lw	a5,12(sp)
  a2b2b4:	880d                	andi	s0,s0,3
  a2b2b6:	041a                	slli	s0,s0,0x6
  a2b2b8:	8bfd                	andi	a5,a5,31
  a2b2ba:	00146413          	ori	s0,s0,1
  a2b2be:	10f4241b          	orshf	s0,s0,a5,sll,8
  a2b2c2:	441137b7          	lui	a5,0x44113
  a2b2c6:	0887a223          	sw	s0,132(a5) # 44113084 <_gp_+0x436de8c8>
  a2b2ca:	0ff0000f          	fence
  a2b2ce:	d7fff0ef          	jal	ra,a2b04c <inner_klad_wait_com_route_done>
  a2b2d2:	c22a                	sw	a0,4(sp)
  a2b2d4:	4792                	lw	a5,4(sp)
  a2b2d6:	f7dd                	bnez	a5,a2b284 <hal_klad_start_com_route+0x2a>
  a2b2d8:	20bc                	lbu	a5,2(s1)
  a2b2da:	d7cd                	beqz	a5,a2b284 <hal_klad_start_com_route+0x2a>
  a2b2dc:	00091563          	bnez	s2,a2b2e6 <hal_klad_start_com_route+0x8c>
  a2b2e0:	4505                	li	a0,1
  a2b2e2:	d4fff0ef          	jal	ra,a2b030 <inner_klad_set_key_odd>
  a2b2e6:	47a2                	lw	a5,8(sp)
  a2b2e8:	f3f47413          	andi	s0,s0,-193
  a2b2ec:	ffff e0ff 071f      	l.li	a4,0xffffe0ff
  a2b2f2:	8b8d                	andi	a5,a5,3
  a2b2f4:	0cf4241b          	orshf	s0,s0,a5,sll,6
  a2b2f8:	47b2                	lw	a5,12(sp)
  a2b2fa:	8c79                	and	s0,s0,a4
  a2b2fc:	8bfd                	andi	a5,a5,31
  a2b2fe:	10f4241b          	orshf	s0,s0,a5,sll,8
  a2b302:	00146413          	ori	s0,s0,1
  a2b306:	441137b7          	lui	a5,0x44113
  a2b30a:	0887a223          	sw	s0,132(a5) # 44113084 <_gp_+0x436de8c8>
  a2b30e:	0ff0000f          	fence
  a2b312:	d3bff0ef          	jal	ra,a2b04c <inner_klad_wait_com_route_done>
  a2b316:	c22a                	sw	a0,4(sp)
  a2b318:	4792                	lw	a5,4(sp)
  a2b31a:	b7ad                	j	a2b284 <hal_klad_start_com_route+0x2a>
  a2b31c:	57fd                	li	a5,-1
  a2b31e:	b7a5                	j	a2b286 <hal_klad_start_com_route+0x2c>

00a2b320 <hal_fapc_set_region_addr>:
  a2b320:	040577bb          	bgeui	a0,4,a2b33e <hal_fapc_set_region_addr+0x1e>
  a2b324:	1200 0400 079f      	l.li	a5,0x12000400
  a2b32a:	953e                	add	a0,a0,a5
  a2b32c:	050a                	slli	a0,a0,0x2
  a2b32e:	c10c                	sw	a1,0(a0)
  a2b330:	0ff0000f          	fence
  a2b334:	c130                	sw	a2,64(a0)
  a2b336:	0ff0000f          	fence
  a2b33a:	4501                	li	a0,0
  a2b33c:	8082                	ret
  a2b33e:	557d                	li	a0,-1
  a2b340:	8082                	ret

00a2b342 <hal_fapc_set_region_permission>:
  a2b342:	4501                	li	a0,0
  a2b344:	8082                	ret

00a2b346 <hal_fapc_set_region_mac_addr>:
  a2b346:	4501                	li	a0,0
  a2b348:	8082                	ret

00a2b34a <hal_fapc_set_region_mode>:
  a2b34a:	0405733b          	bgeui	a0,4,a2b356 <hal_fapc_set_region_mode+0xc>
  a2b34e:	c591                	beqz	a1,a2b35a <hal_fapc_set_region_mode+0x10>
  a2b350:	4709                	li	a4,2
  a2b352:	020582bb          	beqi	a1,2,a2b35c <hal_fapc_set_region_mode+0x12>
  a2b356:	557d                	li	a0,-1
  a2b358:	8082                	ret
  a2b35a:	4701                	li	a4,0
  a2b35c:	480016b7          	lui	a3,0x48001
  a2b360:	1806a603          	lw	a2,384(a3) # 48001180 <_gp_+0x475cc9c4>
  a2b364:	0506                	slli	a0,a0,0x1
  a2b366:	478d                	li	a5,3
  a2b368:	00a797b3          	sll	a5,a5,a0
  a2b36c:	fff7c793          	not	a5,a5
  a2b370:	8ff1                	and	a5,a5,a2
  a2b372:	00a71733          	sll	a4,a4,a0
  a2b376:	8fd9                	or	a5,a5,a4
  a2b378:	18f6a023          	sw	a5,384(a3)
  a2b37c:	0ff0000f          	fence
  a2b380:	4501                	li	a0,0
  a2b382:	8082                	ret

00a2b384 <hal_fapc_region_enable>:
  a2b384:	4501                	li	a0,0
  a2b386:	8082                	ret

00a2b388 <hal_fapc_region_lock>:
  a2b388:	0405783b          	bgeui	a0,4,a2b3a8 <hal_fapc_region_lock+0x20>
  a2b38c:	48001737          	lui	a4,0x48001
  a2b390:	22072683          	lw	a3,544(a4) # 48001220 <_gp_+0x475cca64>
  a2b394:	4785                	li	a5,1
  a2b396:	00a79533          	sll	a0,a5,a0
  a2b39a:	8d55                	or	a0,a0,a3
  a2b39c:	22a72023          	sw	a0,544(a4)
  a2b3a0:	0ff0000f          	fence
  a2b3a4:	4501                	li	a0,0
  a2b3a6:	8082                	ret
  a2b3a8:	557d                	li	a0,-1
  a2b3aa:	8082                	ret

00a2b3ac <hal_fapc_set_region_iv>:
  a2b3ac:	e131                	bnez	a0,a2b3f0 <hal_fapc_set_region_iv+0x44>
  a2b3ae:	557d                	li	a0,-1
  a2b3b0:	c1a9                	beqz	a1,a2b3f2 <hal_fapc_set_region_iv+0x46>
  a2b3b2:	1016103b          	bnei	a2,16,a2b3f2 <hal_fapc_set_region_iv+0x46>
  a2b3b6:	4198                	lw	a4,0(a1)
  a2b3b8:	480017b7          	lui	a5,0x48001
  a2b3bc:	62e7a023          	sw	a4,1568(a5) # 48001620 <_gp_+0x475cce64>
  a2b3c0:	0ff0000f          	fence
  a2b3c4:	41d8                	lw	a4,4(a1)
  a2b3c6:	62e7a223          	sw	a4,1572(a5)
  a2b3ca:	0ff0000f          	fence
  a2b3ce:	4598                	lw	a4,8(a1)
  a2b3d0:	62e7a423          	sw	a4,1576(a5)
  a2b3d4:	0ff0000f          	fence
  a2b3d8:	45d8                	lw	a4,12(a1)
  a2b3da:	62e7a623          	sw	a4,1580(a5)
  a2b3de:	0ff0000f          	fence
  a2b3e2:	4705                	li	a4,1
  a2b3e4:	60e7a023          	sw	a4,1536(a5)
  a2b3e8:	0ff0000f          	fence
  a2b3ec:	4501                	li	a0,0
  a2b3ee:	8082                	ret
  a2b3f0:	557d                	li	a0,-1
  a2b3f2:	8082                	ret

00a2b3f4 <hal_fapc_set_region_iv_start_addr>:
  a2b3f4:	0405763b          	bgeui	a0,4,a2b40c <hal_fapc_set_region_iv_start_addr+0x18>
  a2b3f8:	1200 0590 079f      	l.li	a5,0x12000590
  a2b3fe:	953e                	add	a0,a0,a5
  a2b400:	050a                	slli	a0,a0,0x2
  a2b402:	c10c                	sw	a1,0(a0)
  a2b404:	0ff0000f          	fence
  a2b408:	4501                	li	a0,0
  a2b40a:	8082                	ret
  a2b40c:	557d                	li	a0,-1
  a2b40e:	8082                	ret

00a2b410 <hal_tcxo_init>:
  a2b410:	8018                	push	{ra},-16
  a2b412:	28a9                	jal	ra,a2b46c <hal_tcxo_v150_regs_init>
  a2b414:	e105                	bnez	a0,a2b434 <hal_tcxo_init+0x24>
  a2b416:	00a05737          	lui	a4,0xa05
  a2b41a:	74872683          	lw	a3,1864(a4) # a05748 <g_tcxo_regs>
  a2b41e:	429c                	lw	a5,0(a3)
  a2b420:	0047e793          	ori	a5,a5,4
  a2b424:	c29c                	sw	a5,0(a3)
  a2b426:	74872703          	lw	a4,1864(a4)
  a2b42a:	431c                	lw	a5,0(a4)
  a2b42c:	0027e793          	ori	a5,a5,2
  a2b430:	c31c                	sw	a5,0(a4)
  a2b432:	8014                	popret	{ra},16
  a2b434:	8000 1180 051f      	l.li	a0,0x80001180
  a2b43a:	bfe5                	j	a2b432 <hal_tcxo_init+0x22>

00a2b43c <hal_tcxo_get>:
  a2b43c:	00a057b7          	lui	a5,0xa05
  a2b440:	7487a683          	lw	a3,1864(a5) # a05748 <g_tcxo_regs>
  a2b444:	4605                	li	a2,1
  a2b446:	4298                	lw	a4,0(a3)
  a2b448:	00176713          	ori	a4,a4,1
  a2b44c:	c298                	sw	a4,0(a3)
  a2b44e:	56fd                	li	a3,-1
  a2b450:	873e                	mv	a4,a5
  a2b452:	74872783          	lw	a5,1864(a4)
  a2b456:	439c                	lw	a5,0(a5)
  a2b458:	8391                	srli	a5,a5,0x4
  a2b45a:	8b85                	andi	a5,a5,1
  a2b45c:	00c78763          	beq	a5,a2,a2b46a <hal_tcxo_get+0x2e>
  a2b460:	16fd                	addi	a3,a3,-1
  a2b462:	fae5                	bnez	a3,a2b452 <hal_tcxo_get+0x16>
  a2b464:	4501                	li	a0,0
  a2b466:	4581                	li	a1,0
  a2b468:	8082                	ret
  a2b46a:	a00d                	j	a2b48c <hal_tcxo_reg_count_get>

00a2b46c <hal_tcxo_v150_regs_init>:
  a2b46c:	8018                	push	{ra},-16
  a2b46e:	45b000ef          	jal	ra,a2c0c8 <tcxo_porting_base_addr_get>
  a2b472:	c909                	beqz	a0,a2b484 <hal_tcxo_v150_regs_init+0x18>
  a2b474:	455000ef          	jal	ra,a2c0c8 <tcxo_porting_base_addr_get>
  a2b478:	00a0 5748 079f      	l.li	a5,0xa05748
  a2b47e:	c388                	sw	a0,0(a5)
  a2b480:	4501                	li	a0,0
  a2b482:	8014                	popret	{ra},16
  a2b484:	8000 1180 051f      	l.li	a0,0x80001180
  a2b48a:	bfe5                	j	a2b482 <hal_tcxo_v150_regs_init+0x16>

00a2b48c <hal_tcxo_reg_count_get>:
  a2b48c:	00a0 5748 079f      	l.li	a5,0xa05748
  a2b492:	439c                	lw	a5,0(a5)
  a2b494:	478c                	lw	a1,8(a5)
  a2b496:	43c8                	lw	a0,4(a5)
  a2b498:	8082                	ret

00a2b49a <hal_uart_register_funcs>:
  a2b49a:	030575bb          	bgeui	a0,3,a2b4b0 <hal_uart_register_funcs+0x16>
  a2b49e:	c989                	beqz	a1,a2b4b0 <hal_uart_register_funcs+0x16>
  a2b4a0:	00a0 574c 079f      	l.li	a5,0xa0574c
  a2b4a6:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b4aa:	c10c                	sw	a1,0(a0)
  a2b4ac:	4501                	li	a0,0
  a2b4ae:	8082                	ret
  a2b4b0:	80000537          	lui	a0,0x80000
  a2b4b4:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2b4b6:	8082                	ret

00a2b4b8 <hal_uart_init>:
  a2b4b8:	8128                	push	{ra,s0},-32
  a2b4ba:	842a                	mv	s0,a0
  a2b4bc:	c62e                	sw	a1,12(sp)
  a2b4be:	c432                	sw	a2,8(sp)
  a2b4c0:	c236                	sw	a3,4(sp)
  a2b4c2:	c03a                	sw	a4,0(sp)
  a2b4c4:	347000ef          	jal	ra,a2c00a <uart_port_register_hal_funcs>
  a2b4c8:	00a0 574c 079f      	l.li	a5,0xa0574c
  a2b4ce:	0487879b          	addshf	a5,a5,s0,sll,2
  a2b4d2:	439c                	lw	a5,0(a5)
  a2b4d4:	cb99                	beqz	a5,a2b4ea <hal_uart_init+0x32>
  a2b4d6:	4692                	lw	a3,4(sp)
  a2b4d8:	ca89                	beqz	a3,a2b4ea <hal_uart_init+0x32>
  a2b4da:	0007a303          	lw	t1,0(a5)
  a2b4de:	4702                	lw	a4,0(sp)
  a2b4e0:	4622                	lw	a2,8(sp)
  a2b4e2:	45b2                	lw	a1,12(sp)
  a2b4e4:	8522                	mv	a0,s0
  a2b4e6:	8120                	pop	{ra,s0},32
  a2b4e8:	8302                	jr	t1
  a2b4ea:	8000 1047 051f      	l.li	a0,0x80001047
  a2b4f0:	8124                	popret	{ra,s0},32

00a2b4f2 <hal_uart_write>:
  a2b4f2:	00a0 574c 079f      	l.li	a5,0xa0574c
  a2b4f8:	04a7879b          	addshf	a5,a5,a0,sll,2
  a2b4fc:	439c                	lw	a5,0(a5)
  a2b4fe:	00c7a303          	lw	t1,12(a5)
  a2b502:	8302                	jr	t1

00a2b504 <hal_uart_read>:
  a2b504:	00a0 574c 079f      	l.li	a5,0xa0574c
  a2b50a:	04a7879b          	addshf	a5,a5,a0,sll,2
  a2b50e:	439c                	lw	a5,0(a5)
  a2b510:	0107a303          	lw	t1,16(a5)
  a2b514:	8302                	jr	t1

00a2b516 <hal_uart_ctrl>:
  a2b516:	00a0 574c 079f      	l.li	a5,0xa0574c
  a2b51c:	04a7879b          	addshf	a5,a5,a0,sll,2
  a2b520:	439c                	lw	a5,0(a5)
  a2b522:	c781                	beqz	a5,a2b52a <hal_uart_ctrl+0x14>
  a2b524:	0087a303          	lw	t1,8(a5)
  a2b528:	8302                	jr	t1
  a2b52a:	8000 1047 051f      	l.li	a0,0x80001047
  a2b530:	8082                	ret

00a2b532 <hal_uart_v151_ctrl_get_rxfifo_passnum>:
  a2b532:	c609                	beqz	a2,a2b53c <hal_uart_v151_ctrl_get_rxfifo_passnum+0xa>
  a2b534:	57fd                	li	a5,-1
  a2b536:	c21c                	sw	a5,0(a2)
  a2b538:	4501                	li	a0,0
  a2b53a:	8082                	ret
  a2b53c:	80000537          	lui	a0,0x80000
  a2b540:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2b542:	8082                	ret

00a2b544 <hal_uart_v151_get_uart_rxfifo_threshold>:
  a2b544:	00a0 5774 079f      	l.li	a5,0xa05774
  a2b54a:	953e                	add	a0,a0,a5
  a2b54c:	2114                	lbu	a3,0(a0)
  a2b54e:	03e00713          	li	a4,62
  a2b552:	0306f3bb          	bgeui	a3,3,a2b560 <hal_uart_v151_get_uart_rxfifo_threshold+0x1c>
  a2b556:	00a3 269c 079f      	l.li	a5,0xa3269c
  a2b55c:	97b6                	add	a5,a5,a3
  a2b55e:	2398                	lbu	a4,0(a5)
  a2b560:	c218                	sw	a4,0(a2)
  a2b562:	4501                	li	a0,0
  a2b564:	8082                	ret

00a2b566 <hal_uart_v151_deinit>:
  a2b566:	00a0 5758 079f      	l.li	a5,0xa05758
  a2b56c:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b570:	00052023          	sw	zero,0(a0)
  a2b574:	4501                	li	a0,0
  a2b576:	8082                	ret

00a2b578 <hal_uart_set_diven>:
  a2b578:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b57e:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b582:	4118                	lw	a4,0(a0)
  a2b584:	8985                	andi	a1,a1,1
  a2b586:	471c                	lw	a5,8(a4)
  a2b588:	9bf9                	andi	a5,a5,-2
  a2b58a:	8fcd                	or	a5,a5,a1
  a2b58c:	c71c                	sw	a5,8(a4)
  a2b58e:	8082                	ret

00a2b590 <hal_uart_is_tx_fifo_full>:
  a2b590:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b596:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b59a:	411c                	lw	a5,0(a0)
  a2b59c:	4501                	li	a0,0
  a2b59e:	43fc                	lw	a5,68(a5)
  a2b5a0:	8b85                	andi	a5,a5,1
  a2b5a2:	a21c                	sb	a5,0(a2)
  a2b5a4:	8082                	ret

00a2b5a6 <hal_uart_is_tx_fifo_empty>:
  a2b5a6:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b5ac:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b5b0:	411c                	lw	a5,0(a0)
  a2b5b2:	4501                	li	a0,0
  a2b5b4:	43fc                	lw	a5,68(a5)
  a2b5b6:	8385                	srli	a5,a5,0x1
  a2b5b8:	8b85                	andi	a5,a5,1
  a2b5ba:	a21c                	sb	a5,0(a2)
  a2b5bc:	8082                	ret

00a2b5be <hal_uart_is_busy>:
  a2b5be:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b5c4:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b5c8:	411c                	lw	a5,0(a0)
  a2b5ca:	4501                	li	a0,0
  a2b5cc:	43fc                	lw	a5,68(a5)
  a2b5ce:	9f81                	uxtb	a5
  a2b5d0:	8385                	srli	a5,a5,0x1
  a2b5d2:	0017c793          	xori	a5,a5,1
  a2b5d6:	8b85                	andi	a5,a5,1
  a2b5d8:	a21c                	sb	a5,0(a2)
  a2b5da:	8082                	ret

00a2b5dc <hal_uart_is_rx_fifo_empty>:
  a2b5dc:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b5e2:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b5e6:	411c                	lw	a5,0(a0)
  a2b5e8:	4501                	li	a0,0
  a2b5ea:	43fc                	lw	a5,68(a5)
  a2b5ec:	838d                	srli	a5,a5,0x3
  a2b5ee:	8b85                	andi	a5,a5,1
  a2b5f0:	a21c                	sb	a5,0(a2)
  a2b5f2:	8082                	ret

00a2b5f4 <hal_uart_v151_write>:
  a2b5f4:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b5fa:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b5fe:	e219                	bnez	a2,a2b604 <hal_uart_v151_write+0x10>
  a2b600:	4501                	li	a0,0
  a2b602:	8082                	ret
  a2b604:	4118                	lw	a4,0(a0)
  a2b606:	437c                	lw	a5,68(a4)
  a2b608:	8b85                	andi	a5,a5,1
  a2b60a:	fbf5                	bnez	a5,a2b5fe <hal_uart_v151_write+0xa>
  a2b60c:	0585                	addi	a1,a1,1
  a2b60e:	fff5c783          	lbu	a5,-1(a1)
  a2b612:	167d                	addi	a2,a2,-1 # 44112fff <_gp_+0x436de843>
  a2b614:	9e21                	uxth	a2
  a2b616:	c35c                	sw	a5,4(a4)
  a2b618:	b7dd                	j	a2b5fe <hal_uart_v151_write+0xa>

00a2b61a <hal_uart_v151_read>:
  a2b61a:	00a0 5768 071f      	l.li	a4,0xa05768
  a2b620:	4781                	li	a5,0
  a2b622:	04a7051b          	addshf	a0,a4,a0,sll,2
  a2b626:	00c7c463          	blt	a5,a2,a2b62e <hal_uart_v151_read+0x14>
  a2b62a:	853e                	mv	a0,a5
  a2b62c:	8082                	ret
  a2b62e:	4114                	lw	a3,0(a0)
  a2b630:	42f8                	lw	a4,68(a3)
  a2b632:	8b21                	andi	a4,a4,8
  a2b634:	fb7d                	bnez	a4,a2b62a <hal_uart_v151_read+0x10>
  a2b636:	42d4                	lw	a3,4(a3)
  a2b638:	00f58733          	add	a4,a1,a5
  a2b63c:	0785                	addi	a5,a5,1
  a2b63e:	a314                	sb	a3,0(a4)
  a2b640:	b7dd                	j	a2b626 <hal_uart_v151_read+0xc>

00a2b642 <hal_uart_v151_ctrl>:
  a2b642:	00a3 26a0 079f      	l.li	a5,0xa326a0
  a2b648:	04b7879b          	addshf	a5,a5,a1,sll,2
  a2b64c:	0007a303          	lw	t1,0(a5)
  a2b650:	00030363          	beqz	t1,a2b656 <hal_uart_v151_ctrl+0x14>
  a2b654:	8302                	jr	t1
  a2b656:	8000 1047 051f      	l.li	a0,0x80001047
  a2b65c:	8082                	ret

00a2b65e <hal_uart_init_fifo>:
  a2b65e:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b664:	04a7879b          	addshf	a5,a5,a0,sll,2
  a2b668:	4398                	lw	a4,0(a5)
  a2b66a:	0035f693          	andi	a3,a1,3
  a2b66e:	535c                	lw	a5,36(a4)
  a2b670:	9bcd                	andi	a5,a5,-13
  a2b672:	04d7a79b          	orshf	a5,a5,a3,sll,2
  a2b676:	9bf1                	andi	a5,a5,-4
  a2b678:	00367693          	andi	a3,a2,3
  a2b67c:	8fd5                	or	a5,a5,a3
  a2b67e:	0107e793          	ori	a5,a5,16
  a2b682:	d35c                	sw	a5,36(a4)
  a2b684:	00a0 5774 079f      	l.li	a5,0xa05774
  a2b68a:	97aa                	add	a5,a5,a0
  a2b68c:	a38c                	sb	a1,0(a5)
  a2b68e:	00a0 5778 079f      	l.li	a5,0xa05778
  a2b694:	953e                	add	a0,a0,a5
  a2b696:	a110                	sb	a2,0(a0)
  a2b698:	8082                	ret

00a2b69a <hal_uart_v151_ctrl_set_txfifo_int_level>:
  a2b69a:	0406633b          	bltui	a2,4,a2b6a6 <hal_uart_v151_ctrl_set_txfifo_int_level+0xc>
  a2b69e:	80000537          	lui	a0,0x80000
  a2b6a2:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2b6a4:	8082                	ret
  a2b6a6:	8018                	push	{ra},-16
  a2b6a8:	00a0 5774 079f      	l.li	a5,0xa05774
  a2b6ae:	97aa                	add	a5,a5,a0
  a2b6b0:	238c                	lbu	a1,0(a5)
  a2b6b2:	9e01                	uxtb	a2
  a2b6b4:	376d                	jal	ra,a2b65e <hal_uart_init_fifo>
  a2b6b6:	4501                	li	a0,0
  a2b6b8:	8014                	popret	{ra},16

00a2b6ba <hal_uart_v151_ctrl_set_rxfifo_int_level>:
  a2b6ba:	0406633b          	bltui	a2,4,a2b6c6 <hal_uart_v151_ctrl_set_rxfifo_int_level+0xc>
  a2b6be:	80000537          	lui	a0,0x80000
  a2b6c2:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2b6c4:	8082                	ret
  a2b6c6:	8018                	push	{ra},-16
  a2b6c8:	00a0 5778 079f      	l.li	a5,0xa05778
  a2b6ce:	97aa                	add	a5,a5,a0
  a2b6d0:	85b2                	mv	a1,a2
  a2b6d2:	2390                	lbu	a2,0(a5)
  a2b6d4:	9d81                	uxtb	a1
  a2b6d6:	3761                	jal	ra,a2b65e <hal_uart_init_fifo>
  a2b6d8:	4501                	li	a0,0
  a2b6da:	8014                	popret	{ra},16

00a2b6dc <hal_uart_set_baud_rate>:
  a2b6dc:	c1a1                	beqz	a1,a2b71c <hal_uart_set_baud_rate+0x40>
  a2b6de:	8048                	push	{ra,s0-s2},-16
  a2b6e0:	0592                	slli	a1,a1,0x4
  a2b6e2:	02b67433          	remu	s0,a2,a1
  a2b6e6:	892a                	mv	s2,a0
  a2b6e8:	02b654b3          	divu	s1,a2,a1
  a2b6ec:	041a                	slli	s0,s0,0x6
  a2b6ee:	02b45433          	divu	s0,s0,a1
  a2b6f2:	4585                	li	a1,1
  a2b6f4:	3551                	jal	ra,a2b578 <hal_uart_set_diven>
  a2b6f6:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b6fc:	0527879b          	addshf	a5,a5,s2,sll,2
  a2b700:	439c                	lw	a5,0(a5)
  a2b702:	0ff4f713          	andi	a4,s1,255
  a2b706:	9ca1                	uxth	s1
  a2b708:	cb98                	sw	a4,16(a5)
  a2b70a:	80a1                	srli	s1,s1,0x8
  a2b70c:	c7c4                	sw	s1,12(a5)
  a2b70e:	4581                	li	a1,0
  a2b710:	854a                	mv	a0,s2
  a2b712:	03f47413          	andi	s0,s0,63
  a2b716:	cbc0                	sw	s0,20(a5)
  a2b718:	8040                	pop	{ra,s0-s2},16
  a2b71a:	bdb9                	j	a2b578 <hal_uart_set_diven>
  a2b71c:	8082                	ret

00a2b71e <hal_uart_set_data_bits>:
  a2b71e:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b724:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b728:	4118                	lw	a4,0(a0)
  a2b72a:	898d                	andi	a1,a1,3
  a2b72c:	471c                	lw	a5,8(a4)
  a2b72e:	9bcd                	andi	a5,a5,-13
  a2b730:	04b7a79b          	orshf	a5,a5,a1,sll,2
  a2b734:	c71c                	sw	a5,8(a4)
  a2b736:	8082                	ret

00a2b738 <hal_uart_set_stop_bits>:
  a2b738:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b73e:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b742:	4118                	lw	a4,0(a0)
  a2b744:	8985                	andi	a1,a1,1
  a2b746:	471c                	lw	a5,8(a4)
  a2b748:	f7f7f793          	andi	a5,a5,-129
  a2b74c:	0eb7a79b          	orshf	a5,a5,a1,sll,7
  a2b750:	c71c                	sw	a5,8(a4)
  a2b752:	8082                	ret

00a2b754 <hal_uart_auto_flow_ctl_en>:
  a2b754:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b75a:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b75e:	4118                	lw	a4,0(a0)
  a2b760:	8985                	andi	a1,a1,1
  a2b762:	575c                	lw	a5,44(a4)
  a2b764:	9be9                	andi	a5,a5,-6
  a2b766:	8fcd                	or	a5,a5,a1
  a2b768:	04b7a59b          	orshf	a1,a5,a1,sll,2
  a2b76c:	d74c                	sw	a1,44(a4)
  a2b76e:	8082                	ret

00a2b770 <hal_uart_enable_interrupt>:
  a2b770:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b776:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b77a:	4114                	lw	a3,0(a0)
  a2b77c:	4e9c                	lw	a5,24(a3)
  a2b77e:	0e05f73b          	bgeui	a1,14,a2b79a <hal_uart_enable_interrupt+0x2a>
  a2b782:	00a32737          	lui	a4,0xa32
  a2b786:	66470713          	addi	a4,a4,1636 # a32664 <g_sm3_ival+0x34>
  a2b78a:	04b7059b          	addshf	a1,a4,a1,sll,2
  a2b78e:	4198                	lw	a4,0(a1)
  a2b790:	8702                	jr	a4
  a2b792:	8a05                	andi	a2,a2,1
  a2b794:	9bdd                	andi	a5,a5,-9
  a2b796:	06c7a79b          	orshf	a5,a5,a2,sll,3
  a2b79a:	ce9c                	sw	a5,24(a3)
  a2b79c:	8082                	ret
  a2b79e:	8a05                	andi	a2,a2,1
  a2b7a0:	9bed                	andi	a5,a5,-5
  a2b7a2:	04c7a79b          	orshf	a5,a5,a2,sll,2
  a2b7a6:	bfd5                	j	a2b79a <hal_uart_enable_interrupt+0x2a>
  a2b7a8:	8a05                	andi	a2,a2,1
  a2b7aa:	9bf9                	andi	a5,a5,-2
  a2b7ac:	8fd1                	or	a5,a5,a2
  a2b7ae:	b7f5                	j	a2b79a <hal_uart_enable_interrupt+0x2a>
  a2b7b0:	8a05                	andi	a2,a2,1
  a2b7b2:	9bf5                	andi	a5,a5,-3
  a2b7b4:	02c7a79b          	orshf	a5,a5,a2,sll,1
  a2b7b8:	b7cd                	j	a2b79a <hal_uart_enable_interrupt+0x2a>

00a2b7ba <hal_uart_en_para_err_int>:
  a2b7ba:	8018                	push	{ra},-16
  a2b7bc:	00c03633          	snez	a2,a2
  a2b7c0:	45b5                	li	a1,13
  a2b7c2:	377d                	jal	ra,a2b770 <hal_uart_enable_interrupt>
  a2b7c4:	4501                	li	a0,0
  a2b7c6:	8014                	popret	{ra},16

00a2b7c8 <hal_uart_en_idle_int>:
  a2b7c8:	8018                	push	{ra},-16
  a2b7ca:	00c03633          	snez	a2,a2
  a2b7ce:	45b1                	li	a1,12
  a2b7d0:	3745                	jal	ra,a2b770 <hal_uart_enable_interrupt>
  a2b7d2:	4501                	li	a0,0
  a2b7d4:	8014                	popret	{ra},16

00a2b7d6 <hal_uart_en_rx_int>:
  a2b7d6:	8018                	push	{ra},-16
  a2b7d8:	00c03633          	snez	a2,a2
  a2b7dc:	4591                	li	a1,4
  a2b7de:	3f49                	jal	ra,a2b770 <hal_uart_enable_interrupt>
  a2b7e0:	4501                	li	a0,0
  a2b7e2:	8014                	popret	{ra},16

00a2b7e4 <hal_uart_en_tx_int>:
  a2b7e4:	8018                	push	{ra},-16
  a2b7e6:	00c03633          	snez	a2,a2
  a2b7ea:	4589                	li	a1,2
  a2b7ec:	3751                	jal	ra,a2b770 <hal_uart_enable_interrupt>
  a2b7ee:	4501                	li	a0,0
  a2b7f0:	8014                	popret	{ra},16

00a2b7f2 <hal_uart_check_en>:
  a2b7f2:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b7f8:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b7fc:	4118                	lw	a4,0(a0)
  a2b7fe:	8985                	andi	a1,a1,1
  a2b800:	471c                	lw	a5,8(a4)
  a2b802:	fdf7f793          	andi	a5,a5,-33
  a2b806:	0ab7a79b          	orshf	a5,a5,a1,sll,5
  a2b80a:	c71c                	sw	a5,8(a4)
  a2b80c:	8082                	ret

00a2b80e <hal_uart_parity_en>:
  a2b80e:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b814:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b818:	4118                	lw	a4,0(a0)
  a2b81a:	8985                	andi	a1,a1,1
  a2b81c:	471c                	lw	a5,8(a4)
  a2b81e:	9bbd                	andi	a5,a5,-17
  a2b820:	08b7a79b          	orshf	a5,a5,a1,sll,4
  a2b824:	c71c                	sw	a5,8(a4)
  a2b826:	8082                	ret

00a2b828 <hal_uart_set_parity>:
  a2b828:	8028                	push	{ra,s0},-16
  a2b82a:	842a                	mv	s0,a0
  a2b82c:	0105873b          	beqi	a1,1,a2b848 <hal_uart_set_parity+0x20>
  a2b830:	c581                	beqz	a1,a2b838 <hal_uart_set_parity+0x10>
  a2b832:	020582bb          	beqi	a1,2,a2b83c <hal_uart_set_parity+0x14>
  a2b836:	8024                	popret	{ra,s0},16
  a2b838:	8020                	pop	{ra,s0},16
  a2b83a:	bf65                	j	a2b7f2 <hal_uart_check_en>
  a2b83c:	4585                	li	a1,1
  a2b83e:	3f55                	jal	ra,a2b7f2 <hal_uart_check_en>
  a2b840:	4585                	li	a1,1
  a2b842:	8522                	mv	a0,s0
  a2b844:	8020                	pop	{ra,s0},16
  a2b846:	b7e1                	j	a2b80e <hal_uart_parity_en>
  a2b848:	376d                	jal	ra,a2b7f2 <hal_uart_check_en>
  a2b84a:	4581                	li	a1,0
  a2b84c:	bfdd                	j	a2b842 <hal_uart_set_parity+0x1a>

00a2b84e <hal_uart_v151_ctrl_set_attr>:
  a2b84e:	8138                	push	{ra,s0-s1},-32
  a2b850:	224c                	lbu	a1,4(a2)
  a2b852:	84b2                	mv	s1,a2
  a2b854:	842a                	mv	s0,a0
  a2b856:	35e1                	jal	ra,a2b71e <hal_uart_set_data_bits>
  a2b858:	30cc                	lbu	a1,5(s1)
  a2b85a:	8522                	mv	a0,s0
  a2b85c:	3df1                	jal	ra,a2b738 <hal_uart_set_stop_bits>
  a2b85e:	20ec                	lbu	a1,6(s1)
  a2b860:	8522                	mv	a0,s0
  a2b862:	37d9                	jal	ra,a2b828 <hal_uart_set_parity>
  a2b864:	408c                	lw	a1,0(s1)
  a2b866:	8522                	mv	a0,s0
  a2b868:	c62e                	sw	a1,12(sp)
  a2b86a:	7bc000ef          	jal	ra,a2c026 <uart_port_get_clock_value>
  a2b86e:	45b2                	lw	a1,12(sp)
  a2b870:	862a                	mv	a2,a0
  a2b872:	8522                	mv	a0,s0
  a2b874:	35a5                	jal	ra,a2b6dc <hal_uart_set_baud_rate>
  a2b876:	4501                	li	a0,0
  a2b878:	8134                	popret	{ra,s0-s1},32

00a2b87a <hal_uart_tx_pause_en>:
  a2b87a:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b880:	04a7851b          	addshf	a0,a5,a0,sll,2
  a2b884:	4118                	lw	a4,0(a0)
  a2b886:	8985                	andi	a1,a1,1
  a2b888:	4b3c                	lw	a5,80(a4)
  a2b88a:	9bf9                	andi	a5,a5,-2
  a2b88c:	8fcd                	or	a5,a5,a1
  a2b88e:	cb3c                	sw	a5,80(a4)
  a2b890:	8082                	ret

00a2b892 <hal_uart_v151_init>:
  a2b892:	8158                	push	{ra,s0-s3},-48
  a2b894:	84b6                	mv	s1,a3
  a2b896:	00251913          	slli	s2,a0,0x2
  a2b89a:	00a3 26f4 069f      	l.li	a3,0xa326f4
  a2b8a0:	89ae                	mv	s3,a1
  a2b8a2:	012685b3          	add	a1,a3,s2
  a2b8a6:	418c                	lw	a1,0(a1)
  a2b8a8:	4294                	lw	a3,0(a3)
  a2b8aa:	00a0 5768 079f      	l.li	a5,0xa05768
  a2b8b0:	01278633          	add	a2,a5,s2
  a2b8b4:	c20c                	sw	a1,0(a2)
  a2b8b6:	c394                	sw	a3,0(a5)
  a2b8b8:	20cc                	lbu	a1,4(s1)
  a2b8ba:	00a0 5764 079f      	l.li	a5,0xa05764
  a2b8c0:	97aa                	add	a5,a5,a0
  a2b8c2:	c63a                	sw	a4,12(sp)
  a2b8c4:	00078023          	sb	zero,0(a5)
  a2b8c8:	842a                	mv	s0,a0
  a2b8ca:	e55ff0ef          	jal	ra,a2b71e <hal_uart_set_data_bits>
  a2b8ce:	30cc                	lbu	a1,5(s1)
  a2b8d0:	8522                	mv	a0,s0
  a2b8d2:	359d                	jal	ra,a2b738 <hal_uart_set_stop_bits>
  a2b8d4:	20ec                	lbu	a1,6(s1)
  a2b8d6:	8522                	mv	a0,s0
  a2b8d8:	3f81                	jal	ra,a2b828 <hal_uart_set_parity>
  a2b8da:	4581                	li	a1,0
  a2b8dc:	8522                	mv	a0,s0
  a2b8de:	3f71                	jal	ra,a2b87a <hal_uart_tx_pause_en>
  a2b8e0:	408c                	lw	a1,0(s1)
  a2b8e2:	8522                	mv	a0,s0
  a2b8e4:	c42e                	sw	a1,8(sp)
  a2b8e6:	740000ef          	jal	ra,a2c026 <uart_port_get_clock_value>
  a2b8ea:	45a2                	lw	a1,8(sp)
  a2b8ec:	862a                	mv	a2,a0
  a2b8ee:	8522                	mv	a0,s0
  a2b8f0:	dedff0ef          	jal	ra,a2b6dc <hal_uart_set_baud_rate>
  a2b8f4:	4529                	li	a0,10
  a2b8f6:	99ffe0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2b8fa:	4585                	li	a1,1
  a2b8fc:	4605                	li	a2,1
  a2b8fe:	8522                	mv	a0,s0
  a2b900:	d5fff0ef          	jal	ra,a2b65e <hal_uart_init_fifo>
  a2b904:	00a0 5758 079f      	l.li	a5,0xa05758
  a2b90a:	4732                	lw	a4,12(sp)
  a2b90c:	97ca                	add	a5,a5,s2
  a2b90e:	0137a023          	sw	s3,0(a5)
  a2b912:	4585                	li	a1,1
  a2b914:	030701bb          	beqi	a4,3,a2b91a <hal_uart_v151_init+0x88>
  a2b918:	4581                	li	a1,0
  a2b91a:	8522                	mv	a0,s0
  a2b91c:	e39ff0ef          	jal	ra,a2b754 <hal_uart_auto_flow_ctl_en>
  a2b920:	4501                	li	a0,0
  a2b922:	8154                	popret	{ra,s0-s3},48

00a2b924 <hal_uart_v151_funcs_get>:
  a2b924:	00a3 3e90 051f      	l.li	a0,0xa33e90
  a2b92a:	8082                	ret

00a2b92c <hal_watchdog_register_funcs>:
  a2b92c:	c519                	beqz	a0,a2b93a <hal_watchdog_register_funcs+0xe>
  a2b92e:	00a0 577c 079f      	l.li	a5,0xa0577c
  a2b934:	c388                	sw	a0,0(a5)
  a2b936:	4501                	li	a0,0
  a2b938:	8082                	ret
  a2b93a:	80000537          	lui	a0,0x80000
  a2b93e:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2b940:	8082                	ret

00a2b942 <hal_watchdog_unregister_funcs>:
  a2b942:	00a0 577c 079f      	l.li	a5,0xa0577c
  a2b948:	0007a023          	sw	zero,0(a5)
  a2b94c:	4501                	li	a0,0
  a2b94e:	8082                	ret

00a2b950 <hal_watchdog_get_funcs>:
  a2b950:	00a0 577c 079f      	l.li	a5,0xa0577c
  a2b956:	4388                	lw	a0,0(a5)
  a2b958:	8082                	ret

00a2b95a <hal_watchdog_regs_init>:
  a2b95a:	00a347b7          	lui	a5,0xa34
  a2b95e:	f287a783          	lw	a5,-216(a5) # a33f28 <g_watchdog_base_addr>
  a2b962:	c799                	beqz	a5,a2b970 <hal_watchdog_regs_init+0x16>
  a2b964:	00a0 5780 071f      	l.li	a4,0xa05780
  a2b96a:	c31c                	sw	a5,0(a4)
  a2b96c:	4501                	li	a0,0
  a2b96e:	8082                	ret
  a2b970:	8000 1200 051f      	l.li	a0,0x80001200
  a2b976:	8082                	ret

00a2b978 <hal_watchdog_regs_deinit>:
  a2b978:	00a0 5780 079f      	l.li	a5,0xa05780
  a2b97e:	0007a023          	sw	zero,0(a5)
  a2b982:	8082                	ret

00a2b984 <hal_watchdog_v151_kick>:
  a2b984:	00a057b7          	lui	a5,0xa05
  a2b988:	7807a783          	lw	a5,1920(a5) # a05780 <g_watchdog_regs>
  a2b98c:	5a5a 5a5a 071f      	l.li	a4,0x5a5a5a5a
  a2b992:	c798                	sw	a4,8(a5)
  a2b994:	8082                	ret

00a2b996 <hal_watchdog_v151_get_attr>:
  a2b996:	00a0 5788 079f      	l.li	a5,0xa05788
  a2b99c:	4388                	lw	a0,0(a5)
  a2b99e:	8082                	ret

00a2b9a0 <hal_register_watchdog_v151_callback>:
  a2b9a0:	00a0 5784 079f      	l.li	a5,0xa05784
  a2b9a6:	c388                	sw	a0,0(a5)
  a2b9a8:	8082                	ret

00a2b9aa <hal_watchdog_v151_deinit>:
  a2b9aa:	b7f9                	j	a2b978 <hal_watchdog_regs_deinit>

00a2b9ac <hal_watchdog_v151_init>:
  a2b9ac:	b77d                	j	a2b95a <hal_watchdog_regs_init>

00a2b9ae <hal_watchdog_v151_get_left_time>:
  a2b9ae:	8038                	push	{ra,s0-s1},-16
  a2b9b0:	00a057b7          	lui	a5,0xa05
  a2b9b4:	7807a683          	lw	a3,1920(a5) # a05780 <g_watchdog_regs>
  a2b9b8:	7d100493          	li	s1,2001
  a2b9bc:	843e                	mv	s0,a5
  a2b9be:	5698                	lw	a4,40(a3)
  a2b9c0:	00176713          	ori	a4,a4,1
  a2b9c4:	d698                	sw	a4,40(a3)
  a2b9c6:	78042703          	lw	a4,1920(s0)
  a2b9ca:	571c                	lw	a5,40(a4)
  a2b9cc:	8385                	srli	a5,a5,0x1
  a2b9ce:	8b85                	andi	a5,a5,1
  a2b9d0:	cf99                	beqz	a5,a2b9ee <hal_watchdog_v151_get_left_time+0x40>
  a2b9d2:	4b40                	lw	s0,20(a4)
  a2b9d4:	738000ef          	jal	ra,a2c10c <watchdog_port_get_clock>
  a2b9d8:	3e800793          	li	a5,1000
  a2b9dc:	862a                	mv	a2,a0
  a2b9de:	02f435b3          	mulhu	a1,s0,a5
  a2b9e2:	4681                	li	a3,0
  a2b9e4:	02f40533          	mul	a0,s0,a5
  a2b9e8:	bb5fc0ef          	jal	ra,a2859c <__udivdi3>
  a2b9ec:	8034                	popret	{ra,s0-s1},16
  a2b9ee:	14fd                	addi	s1,s1,-1
  a2b9f0:	c489                	beqz	s1,a2b9fa <hal_watchdog_v151_get_left_time+0x4c>
  a2b9f2:	4505                	li	a0,1
  a2b9f4:	8a1fe0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2b9f8:	b7f9                	j	a2b9c6 <hal_watchdog_v151_get_left_time+0x18>
  a2b9fa:	4501                	li	a0,0
  a2b9fc:	bfc5                	j	a2b9ec <hal_watchdog_v151_get_left_time+0x3e>

00a2b9fe <hal_watchdog_v151_clear_interrupt>:
  a2b9fe:	00a057b7          	lui	a5,0xa05
  a2ba02:	7807a703          	lw	a4,1920(a5) # a05780 <g_watchdog_regs>
  a2ba06:	475c                	lw	a5,12(a4)
  a2ba08:	0017e793          	ori	a5,a5,1
  a2ba0c:	c75c                	sw	a5,12(a4)
  a2ba0e:	8082                	ret

00a2ba10 <hal_watchdog_v151_disable>:
  a2ba10:	8018                	push	{ra},-16
  a2ba12:	37f5                	jal	ra,a2b9fe <hal_watchdog_v151_clear_interrupt>
  a2ba14:	00a057b7          	lui	a5,0xa05
  a2ba18:	7807a703          	lw	a4,1920(a5) # a05780 <g_watchdog_regs>
  a2ba1c:	4b1c                	lw	a5,16(a4)
  a2ba1e:	9bf9                	andi	a5,a5,-2
  a2ba20:	cb1c                	sw	a5,16(a4)
  a2ba22:	8014                	popret	{ra},16

00a2ba24 <hal_watchdog_v151_set_attr>:
  a2ba24:	8048                	push	{ra,s0-s2},-16
  a2ba26:	84aa                	mv	s1,a0
  a2ba28:	6e4000ef          	jal	ra,a2c10c <watchdog_port_get_clock>
  a2ba2c:	e489                	bnez	s1,a2ba36 <hal_watchdog_v151_set_attr+0x12>
  a2ba2e:	80000537          	lui	a0,0x80000
  a2ba32:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2ba34:	8044                	popret	{ra,s0-s2},16
  a2ba36:	02950433          	mul	s0,a0,s1
  a2ba3a:	02945933          	divu	s2,s0,s1
  a2ba3e:	6ce000ef          	jal	ra,a2c10c <watchdog_port_get_clock>
  a2ba42:	fea916e3          	bne	s2,a0,a2ba2e <hal_watchdog_v151_set_attr+0xa>
  a2ba46:	fff46a3b          	bltui	s0,255,a2ba2e <hal_watchdog_v151_set_attr+0xa>
  a2ba4a:	00a0 5788 079f      	l.li	a5,0xa05788
  a2ba50:	00a05937          	lui	s2,0xa05
  a2ba54:	c384                	sw	s1,0(a5)
  a2ba56:	78092783          	lw	a5,1920(s2) # a05780 <g_watchdog_regs>
  a2ba5a:	5a5a 5a5a 049f      	l.li	s1,0x5a5a5a5a
  a2ba60:	f0047413          	andi	s0,s0,-256
  a2ba64:	c384                	sw	s1,0(a5)
  a2ba66:	376d                	jal	ra,a2ba10 <hal_watchdog_v151_disable>
  a2ba68:	78092783          	lw	a5,1920(s2)
  a2ba6c:	4b98                	lw	a4,16(a5)
  a2ba6e:	00476713          	ori	a4,a4,4
  a2ba72:	cb98                	sw	a4,16(a5)
  a2ba74:	4b98                	lw	a4,16(a5)
  a2ba76:	fbf77713          	andi	a4,a4,-65
  a2ba7a:	cb98                	sw	a4,16(a5)
  a2ba7c:	4b98                	lw	a4,16(a5)
  a2ba7e:	03876713          	ori	a4,a4,56
  a2ba82:	cb98                	sw	a4,16(a5)
  a2ba84:	43c8                	lw	a0,4(a5)
  a2ba86:	0ff57513          	andi	a0,a0,255
  a2ba8a:	8c49                	or	s0,s0,a0
  a2ba8c:	c3c0                	sw	s0,4(a5)
  a2ba8e:	c784                	sw	s1,8(a5)
  a2ba90:	4501                	li	a0,0
  a2ba92:	b74d                	j	a2ba34 <hal_watchdog_v151_set_attr+0x10>

00a2ba94 <hal_watchdog_v151_enable>:
  a2ba94:	8128                	push	{ra,s0},-32
  a2ba96:	c62a                	sw	a0,12(sp)
  a2ba98:	3fa5                	jal	ra,a2ba10 <hal_watchdog_v151_disable>
  a2ba9a:	4532                	lw	a0,12(sp)
  a2ba9c:	00a05437          	lui	s0,0xa05
  a2baa0:	e105                	bnez	a0,a2bac0 <hal_watchdog_v151_enable+0x2c>
  a2baa2:	78042703          	lw	a4,1920(s0) # a05780 <g_watchdog_regs>
  a2baa6:	4b1c                	lw	a5,16(a4)
  a2baa8:	f7f7f793          	andi	a5,a5,-129
  a2baac:	cb1c                	sw	a5,16(a4)
  a2baae:	3f81                	jal	ra,a2b9fe <hal_watchdog_v151_clear_interrupt>
  a2bab0:	78042703          	lw	a4,1920(s0)
  a2bab4:	4b1c                	lw	a5,16(a4)
  a2bab6:	0017e793          	ori	a5,a5,1
  a2baba:	cb1c                	sw	a5,16(a4)
  a2babc:	8120                	pop	{ra,s0},32
  a2babe:	b5d9                	j	a2b984 <hal_watchdog_v151_kick>
  a2bac0:	01f51bbb          	bnei	a0,1,a2baae <hal_watchdog_v151_enable+0x1a>
  a2bac4:	78042703          	lw	a4,1920(s0)
  a2bac8:	4b1c                	lw	a5,16(a4)
  a2baca:	0807e793          	ori	a5,a5,128
  a2bace:	bff9                	j	a2baac <hal_watchdog_v151_enable+0x18>

00a2bad0 <hal_watchdog_v151_funcs_get>:
  a2bad0:	00a3 3ea8 051f      	l.li	a0,0xa33ea8
  a2bad6:	8082                	ret

00a2bad8 <hal_sfc_regs_init>:
  a2bad8:	8058                	push	{ra,s0-s3},-32
  a2bada:	25ad                	jal	ra,a2c144 <sfc_port_get_sfc_global_conf_base_addr>
  a2badc:	00a0 579c 099f      	l.li	s3,0xa0579c
  a2bae2:	00a9a023          	sw	a0,0(s3)
  a2bae6:	00a0 5790 091f      	l.li	s2,0xa05790
  a2baec:	2585                	jal	ra,a2c14c <sfc_port_get_sfc_bus_regs_base_addr>
  a2baee:	00a92023          	sw	a0,0(s2)
  a2baf2:	00a0 578c 049f      	l.li	s1,0xa0578c
  a2baf8:	2db1                	jal	ra,a2c154 <sfc_port_get_sfc_bus_dma_regs_base_addr>
  a2bafa:	c088                	sw	a0,0(s1)
  a2bafc:	00a0 5798 041f      	l.li	s0,0xa05798
  a2bb02:	2da9                	jal	ra,a2c15c <sfc_port_get_sfc_cmd_regs_base_addr>
  a2bb04:	c008                	sw	a0,0(s0)
  a2bb06:	2db9                	jal	ra,a2c164 <sfc_port_get_sfc_cmd_databuf_base_addr>
  a2bb08:	00a0 5794 079f      	l.li	a5,0xa05794
  a2bb0e:	c388                	sw	a0,0(a5)
  a2bb10:	0009a783          	lw	a5,0(s3)
  a2bb14:	cb99                	beqz	a5,a2bb2a <hal_sfc_regs_init+0x52>
  a2bb16:	00092783          	lw	a5,0(s2)
  a2bb1a:	cb81                	beqz	a5,a2bb2a <hal_sfc_regs_init+0x52>
  a2bb1c:	409c                	lw	a5,0(s1)
  a2bb1e:	c791                	beqz	a5,a2bb2a <hal_sfc_regs_init+0x52>
  a2bb20:	401c                	lw	a5,0(s0)
  a2bb22:	c781                	beqz	a5,a2bb2a <hal_sfc_regs_init+0x52>
  a2bb24:	c119                	beqz	a0,a2bb2a <hal_sfc_regs_init+0x52>
  a2bb26:	4501                	li	a0,0
  a2bb28:	8054                	popret	{ra,s0-s3},32
  a2bb2a:	8000 1343 051f      	l.li	a0,0x80001343
  a2bb30:	bfe5                	j	a2bb28 <hal_sfc_regs_init+0x50>

00a2bb32 <hal_sfc_regs_set_cmd_addr>:
  a2bb32:	00a057b7          	lui	a5,0xa05
  a2bb36:	7987a683          	lw	a3,1944(a5) # a05798 <g_sfc_cmd_regs>
  a2bb3a:	3fff ffff 071f      	l.li	a4,0x3fffffff
  a2bb40:	8d79                	and	a0,a0,a4
  a2bb42:	46dc                	lw	a5,12(a3)
  a2bb44:	c0000737          	lui	a4,0xc0000
  a2bb48:	8ff9                	and	a5,a5,a4
  a2bb4a:	8fc9                	or	a5,a5,a0
  a2bb4c:	c6dc                	sw	a5,12(a3)
  a2bb4e:	8082                	ret

00a2bb50 <hal_sfc_regs_wait_ready.constprop.3>:
  a2bb50:	8158                	push	{ra,s0-s3},-48
  a2bb52:	2515                	jal	ra,a2c176 <sfc_port_get_delay_times>
  a2bb54:	84aa                	mv	s1,a0
  a2bb56:	2d19                	jal	ra,a2c16c <sfc_port_get_delay_once_time>
  a2bb58:	4792                	lw	a5,4(sp)
  a2bb5a:	ffff c7ff 071f      	l.li	a4,0xffffc7ff
  a2bb60:	892a                	mv	s2,a0
  a2bb62:	8077f793          	andi	a5,a5,-2041
  a2bb66:	0287e793          	ori	a5,a5,40
  a2bb6a:	8ff9                	and	a5,a5,a4
  a2bb6c:	c23e                	sw	a5,4(sp)
  a2bb6e:	4785                	li	a5,1
  a2bb70:	c43e                	sw	a5,8(sp)
  a2bb72:	c602                	sw	zero,12(sp)
  a2bb74:	4401                	li	s0,0
  a2bb76:	00a059b7          	lui	s3,0xa05
  a2bb7a:	0048                	addi	a0,sp,4
  a2bb7c:	269d                	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2bb7e:	4601                	li	a2,0
  a2bb80:	4585                	li	a1,1
  a2bb82:	4505                	li	a0,1
  a2bb84:	2e59                	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2bb86:	26c9                	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2bb88:	7949a783          	lw	a5,1940(s3) # a05794 <g_sfc_cmd_databuf>
  a2bb8c:	439c                	lw	a5,0(a5)
  a2bb8e:	8b85                	andi	a5,a5,1
  a2bb90:	cf91                	beqz	a5,a2bbac <hal_sfc_regs_wait_ready.constprop.3+0x5c>
  a2bb92:	854a                	mv	a0,s2
  a2bb94:	f00fe0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2bb98:	00140793          	addi	a5,s0,1
  a2bb9c:	00946663          	bltu	s0,s1,a2bba8 <hal_sfc_regs_wait_ready.constprop.3+0x58>
  a2bba0:	8000 1344 051f      	l.li	a0,0x80001344
  a2bba6:	8154                	popret	{ra,s0-s3},48
  a2bba8:	843e                	mv	s0,a5
  a2bbaa:	bfc1                	j	a2bb7a <hal_sfc_regs_wait_ready.constprop.3+0x2a>
  a2bbac:	4501                	li	a0,0
  a2bbae:	bfe5                	j	a2bba6 <hal_sfc_regs_wait_ready.constprop.3+0x56>

00a2bbb0 <hal_sfc_execute_type_cmd>:
  a2bbb0:	8148                	push	{ra,s0-s2},-32
  a2bbb2:	219c                	lbu	a5,0(a1)
  a2bbb4:	842a                	mv	s0,a0
  a2bbb6:	892e                	mv	s2,a1
  a2bbb8:	078e                	slli	a5,a5,0x3
  a2bbba:	0017e793          	ori	a5,a5,1
  a2bbbe:	c23e                	sw	a5,4(sp)
  a2bbc0:	fff50793          	addi	a5,a0,-1
  a2bbc4:	c43e                	sw	a5,8(sp)
  a2bbc6:	c602                	sw	zero,12(sp)
  a2bbc8:	3761                	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bbca:	84aa                	mv	s1,a0
  a2bbcc:	e50d                	bnez	a0,a2bbf6 <hal_sfc_execute_type_cmd+0x46>
  a2bbce:	0048                	addi	a0,sp,4
  a2bbd0:	2e09                	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2bbd2:	4785                	li	a5,1
  a2bbd4:	c002                	sw	zero,0(sp)
  a2bbd6:	0287e263          	bltu	a5,s0,a2bbfa <hal_sfc_execute_type_cmd+0x4a>
  a2bbda:	00a057b7          	lui	a5,0xa05
  a2bbde:	7947a783          	lw	a5,1940(a5) # a05794 <g_sfc_cmd_databuf>
  a2bbe2:	4702                	lw	a4,0(sp)
  a2bbe4:	00243593          	sltiu	a1,s0,2
  a2bbe8:	4601                	li	a2,0
  a2bbea:	c398                	sw	a4,0(a5)
  a2bbec:	0015c593          	xori	a1,a1,1
  a2bbf0:	4501                	li	a0,0
  a2bbf2:	2625                	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2bbf4:	2e91                	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2bbf6:	8526                	mv	a0,s1
  a2bbf8:	8144                	popret	{ra,s0-s2},32
  a2bbfa:	00f906b3          	add	a3,s2,a5
  a2bbfe:	2294                	lbu	a3,0(a3)
  a2bc00:	00f10733          	add	a4,sp,a5
  a2bc04:	0785                	addi	a5,a5,1
  a2bc06:	fed70fa3          	sb	a3,-1(a4) # bfffffff <_gp_+0xbf5cb843>
  a2bc0a:	b7f1                	j	a2bbd6 <hal_sfc_execute_type_cmd+0x26>

00a2bc0c <hal_sfc_write_enable>:
  a2bc0c:	8118                	push	{ra},-32
  a2bc0e:	3789                	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bc10:	4799                	li	a5,6
  a2bc12:	006c                	addi	a1,sp,12
  a2bc14:	4505                	li	a0,1
  a2bc16:	00f10623          	sb	a5,12(sp)
  a2bc1a:	3f59                	jal	ra,a2bbb0 <hal_sfc_execute_type_cmd>
  a2bc1c:	e111                	bnez	a0,a2bc20 <hal_sfc_write_enable+0x14>
  a2bc1e:	3f0d                	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bc20:	8114                	popret	{ra},32

00a2bc22 <hal_sfc_reg_write_once>:
  a2bc22:	8648                	push	{ra,s0-s2},-112
  a2bc24:	c636                	sw	a3,12(sp)
  a2bc26:	892a                	mv	s2,a0
  a2bc28:	84ae                	mv	s1,a1
  a2bc2a:	8432                	mv	s0,a2
  a2bc2c:	37c5                	jal	ra,a2bc0c <hal_sfc_write_enable>
  a2bc2e:	87aa                	mv	a5,a0
  a2bc30:	46b2                	lw	a3,12(sp)
  a2bc32:	e939                	bnez	a0,a2bc88 <hal_sfc_reg_write_once+0x66>
  a2bc34:	ca36                	sw	a3,20(sp)
  a2bc36:	0848                	addi	a0,sp,20
  a2bc38:	82b9                	srli	a3,a3,0xe
  a2bc3a:	ce36                	sw	a3,28(sp)
  a2bc3c:	cc22                	sw	s0,24(sp)
  a2bc3e:	2455                	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2bc40:	854a                	mv	a0,s2
  a2bc42:	ef1ff0ef          	jal	ra,a2bb32 <hal_sfc_regs_set_cmd_addr>
  a2bc46:	86a2                	mv	a3,s0
  a2bc48:	8626                	mv	a2,s1
  a2bc4a:	04000593          	li	a1,64
  a2bc4e:	1008                	addi	a0,sp,32
  a2bc50:	521040ef          	jal	ra,a30970 <memcpy_s>
  a2bc54:	57fd                	li	a5,-1
  a2bc56:	e90d                	bnez	a0,a2bc88 <hal_sfc_reg_write_once+0x66>
  a2bc58:	00347613          	andi	a2,s0,3
  a2bc5c:	00c03633          	snez	a2,a2
  a2bc60:	4486061b          	addshf	a2,a2,s0,srl,2
  a2bc64:	9e21                	uxth	a2
  a2bc66:	101c                	addi	a5,sp,32
  a2bc68:	4701                	li	a4,0
  a2bc6a:	00a055b7          	lui	a1,0xa05
  a2bc6e:	01071693          	slli	a3,a4,0x10
  a2bc72:	82c1                	srli	a3,a3,0x10
  a2bc74:	00c6ec63          	bltu	a3,a2,a2bc8c <hal_sfc_reg_write_once+0x6a>
  a2bc78:	4605                	li	a2,1
  a2bc7a:	4585                	li	a1,1
  a2bc7c:	4501                	li	a0,0
  a2bc7e:	2c71                	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2bc80:	24e1                	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2bc82:	ecfff0ef          	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bc86:	87aa                	mv	a5,a0
  a2bc88:	853e                	mv	a0,a5
  a2bc8a:	8644                	popret	{ra,s0-s2},112
  a2bc8c:	7945a683          	lw	a3,1940(a1) # a05794 <g_sfc_cmd_databuf>
  a2bc90:	4388                	lw	a0,0(a5)
  a2bc92:	0791                	addi	a5,a5,4
  a2bc94:	04e6869b          	addshf	a3,a3,a4,sll,2
  a2bc98:	c288                	sw	a0,0(a3)
  a2bc9a:	0705                	addi	a4,a4,1
  a2bc9c:	bfc9                	j	a2bc6e <hal_sfc_reg_write_once+0x4c>

00a2bc9e <hal_sfc_get_flash_id>:
  a2bc9e:	8128                	push	{ra,s0},-32
  a2bca0:	842a                	mv	s0,a0
  a2bca2:	e37ff0ef          	jal	ra,a2bad8 <hal_sfc_regs_init>
  a2bca6:	eabff0ef          	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bcaa:	e91d                	bnez	a0,a2bce0 <hal_sfc_get_flash_id+0x42>
  a2bcac:	4631                	li	a2,12
  a2bcae:	00a3 1380 059f      	l.li	a1,0xa31380
  a2bcb4:	0048                	addi	a0,sp,4
  a2bcb6:	d9dfc0ef          	jal	ra,a28a52 <memcpy>
  a2bcba:	0048                	addi	a0,sp,4
  a2bcbc:	241d                	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2bcbe:	4601                	li	a2,0
  a2bcc0:	4585                	li	a1,1
  a2bcc2:	4505                	li	a0,1
  a2bcc4:	2c99                	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2bcc6:	2449                	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2bcc8:	00a057b7          	lui	a5,0xa05
  a2bccc:	7947a783          	lw	a5,1940(a5) # a05794 <g_sfc_cmd_databuf>
  a2bcd0:	00ff ffff 071f      	l.li	a4,0xffffff
  a2bcd6:	439c                	lw	a5,0(a5)
  a2bcd8:	8ff9                	and	a5,a5,a4
  a2bcda:	c01c                	sw	a5,0(s0)
  a2bcdc:	e75ff0ef          	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bce0:	8124                	popret	{ra,s0},32

00a2bce2 <hal_sfc_init>:
  a2bce2:	67c1                	lui	a5,0x10
  a2bce4:	0cf66c63          	bltu	a2,a5,a2bdbc <hal_sfc_init+0xda>
  a2bce8:	8198                	push	{ra,s0-s7},-64
  a2bcea:	4940                	lw	s0,20(a0)
  a2bcec:	84b2                	mv	s1,a2
  a2bcee:	89ae                	mv	s3,a1
  a2bcf0:	892a                	mv	s2,a0
  a2bcf2:	4a05                	li	s4,1
  a2bcf4:	00a05ab7          	lui	s5,0xa05
  a2bcf8:	e025                	bnez	s0,a2bd58 <hal_sfc_init+0x76>
  a2bcfa:	00a057b7          	lui	a5,0xa05
  a2bcfe:	7907a683          	lw	a3,1936(a5) # a05790 <g_sfc_bus_regs>
  a2bd02:	0109d993          	srli	s3,s3,0x10
  a2bd06:	00f4d613          	srli	a2,s1,0xf
  a2bd0a:	4e9c                	lw	a5,24(a3)
  a2bd0c:	4701                	li	a4,0
  a2bd0e:	01079593          	slli	a1,a5,0x10
  a2bd12:	81c1                	srli	a1,a1,0x10
  a2bd14:	2135a59b          	orshf	a1,a1,s3,sll,16
  a2bd18:	ce8c                	sw	a1,24(a3)
  a2bd1a:	00167793          	andi	a5,a2,1
  a2bd1e:	cfc1                	beqz	a5,a2bdb6 <hal_sfc_init+0xd4>
  a2bd20:	4a9c                	lw	a5,16(a3)
  a2bd22:	ffff f0ff 061f      	l.li	a2,0xfffff0ff
  a2bd28:	8b3d                	andi	a4,a4,15
  a2bd2a:	8ff1                	and	a5,a5,a2
  a2bd2c:	10e7a79b          	orshf	a5,a5,a4,sll,8
  a2bd30:	ca9c                	sw	a5,16(a3)
  a2bd32:	00492503          	lw	a0,4(s2)
  a2bd36:	240d                	jal	ra,a2bf58 <hal_sfc_regs_set_bus_read>
  a2bd38:	00c92503          	lw	a0,12(s2)
  a2bd3c:	7f857793          	andi	a5,a0,2040
  a2bd40:	c391                	beqz	a5,a2bd44 <hal_sfc_init+0x62>
  a2bd42:	2c89                	jal	ra,a2bf94 <hal_sfc_regs_set_bus_write>
  a2bd44:	00a057b7          	lui	a5,0xa05
  a2bd48:	79c7a783          	lw	a5,1948(a5) # a0579c <g_sfc_global_conf_regs>
  a2bd4c:	4705                	li	a4,1
  a2bd4e:	cb98                	sw	a4,16(a5)
  a2bd50:	e01ff0ef          	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bd54:	4501                	li	a0,0
  a2bd56:	a00d                	j	a2bd78 <hal_sfc_init+0x96>
  a2bd58:	201c                	lbu	a5,0(s0)
  a2bd5a:	03478063          	beq	a5,s4,a2bd7a <hal_sfc_init+0x98>
  a2bd5e:	c799                	beqz	a5,a2bd6c <hal_sfc_init+0x8a>
  a2bd60:	02e786bb          	beqi	a5,2,a2bcfa <hal_sfc_init+0x18>
  a2bd64:	8000 1345 051f      	l.li	a0,0x80001345
  a2bd6a:	a039                	j	a2bd78 <hal_sfc_init+0x96>
  a2bd6c:	3008                	lbu	a0,1(s0)
  a2bd6e:	00240593          	addi	a1,s0,2
  a2bd72:	e3fff0ef          	jal	ra,a2bbb0 <hal_sfc_execute_type_cmd>
  a2bd76:	cd15                	beqz	a0,a2bdb2 <hal_sfc_init+0xd0>
  a2bd78:	8194                	popret	{ra,s0-s7},64
  a2bd7a:	2038                	lbu	a4,2(s0)
  a2bd7c:	00344b83          	lbu	s7,3(s0)
  a2bd80:	00444b03          	lbu	s6,4(s0)
  a2bd84:	070e                	slli	a4,a4,0x3
  a2bd86:	00176713          	ori	a4,a4,1
  a2bd8a:	c23a                	sw	a4,4(sp)
  a2bd8c:	c452                	sw	s4,8(sp)
  a2bd8e:	c602                	sw	zero,12(sp)
  a2bd90:	dc1ff0ef          	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bd94:	0048                	addi	a0,sp,4
  a2bd96:	22b1                	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2bd98:	4601                	li	a2,0
  a2bd9a:	4585                	li	a1,1
  a2bd9c:	4505                	li	a0,1
  a2bd9e:	2ab5                	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2bda0:	2265                	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2bda2:	794aa783          	lw	a5,1940(s5) # a05794 <g_sfc_cmd_databuf>
  a2bda6:	439c                	lw	a5,0(a5)
  a2bda8:	0177d7b3          	srl	a5,a5,s7
  a2bdac:	8b85                	andi	a5,a5,1
  a2bdae:	fb679be3          	bne	a5,s6,a2bd64 <hal_sfc_init+0x82>
  a2bdb2:	0419                	addi	s0,s0,6
  a2bdb4:	b791                	j	a2bcf8 <hal_sfc_init+0x16>
  a2bdb6:	8205                	srli	a2,a2,0x1
  a2bdb8:	0705                	addi	a4,a4,1
  a2bdba:	b785                	j	a2bd1a <hal_sfc_init+0x38>
  a2bdbc:	80000537          	lui	a0,0x80000
  a2bdc0:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2bdc2:	8082                	ret

00a2bdc4 <hal_sfc_reg_read>:
  a2bdc4:	8588                	push	{ra,s0-s6},-112
  a2bdc6:	89aa                	mv	s3,a0
  a2bdc8:	8a2e                	mv	s4,a1
  a2bdca:	84b2                	mv	s1,a2
  a2bdcc:	04000413          	li	s0,64
  a2bdd0:	00e6da93          	srli	s5,a3,0xe
  a2bdd4:	8936                	mv	s2,a3
  a2bdd6:	00a05b37          	lui	s6,0xa05
  a2bdda:	e099                	bnez	s1,a2bde0 <hal_sfc_reg_read+0x1c>
  a2bddc:	4501                	li	a0,0
  a2bdde:	a8a1                	j	a2be36 <hal_sfc_reg_read+0x72>
  a2bde0:	d71ff0ef          	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bde4:	e929                	bnez	a0,a2be36 <hal_sfc_reg_read+0x72>
  a2bde6:	0084f363          	bgeu	s1,s0,a2bdec <hal_sfc_reg_read+0x28>
  a2bdea:	8426                	mv	s0,s1
  a2bdec:	0048                	addi	a0,sp,4
  a2bdee:	c24a                	sw	s2,4(sp)
  a2bdf0:	c422                	sw	s0,8(sp)
  a2bdf2:	c656                	sw	s5,12(sp)
  a2bdf4:	20fd                	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2bdf6:	854e                	mv	a0,s3
  a2bdf8:	d3bff0ef          	jal	ra,a2bb32 <hal_sfc_regs_set_cmd_addr>
  a2bdfc:	4605                	li	a2,1
  a2bdfe:	4505                	li	a0,1
  a2be00:	4585                	li	a1,1
  a2be02:	2a21                	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2be04:	2291                	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2be06:	00347793          	andi	a5,s0,3
  a2be0a:	00f037b3          	snez	a5,a5
  a2be0e:	794b2503          	lw	a0,1940(s6) # a05794 <g_sfc_cmd_databuf>
  a2be12:	4487879b          	addshf	a5,a5,s0,srl,2
  a2be16:	0814                	addi	a3,sp,16
  a2be18:	9fa1                	uxth	a5
  a2be1a:	4701                	li	a4,0
  a2be1c:	8636                	mv	a2,a3
  a2be1e:	01071593          	slli	a1,a4,0x10
  a2be22:	81c1                	srli	a1,a1,0x10
  a2be24:	00f5ea63          	bltu	a1,a5,a2be38 <hal_sfc_reg_read+0x74>
  a2be28:	86a2                	mv	a3,s0
  a2be2a:	85a2                	mv	a1,s0
  a2be2c:	8552                	mv	a0,s4
  a2be2e:	343040ef          	jal	ra,a30970 <memcpy_s>
  a2be32:	c919                	beqz	a0,a2be48 <hal_sfc_reg_read+0x84>
  a2be34:	557d                	li	a0,-1
  a2be36:	8584                	popret	{ra,s0-s6},112
  a2be38:	04e5059b          	addshf	a1,a0,a4,sll,2
  a2be3c:	418c                	lw	a1,0(a1)
  a2be3e:	0705                	addi	a4,a4,1
  a2be40:	0691                	addi	a3,a3,4
  a2be42:	feb6ae23          	sw	a1,-4(a3)
  a2be46:	bfe1                	j	a2be1e <hal_sfc_reg_read+0x5a>
  a2be48:	99a2                	add	s3,s3,s0
  a2be4a:	9a22                	add	s4,s4,s0
  a2be4c:	8c81                	sub	s1,s1,s0
  a2be4e:	b771                	j	a2bdda <hal_sfc_reg_read+0x16>

00a2be50 <hal_sfc_reg_write>:
  a2be50:	8088                	push	{ra,s0-s6},-32
  a2be52:	03f57793          	andi	a5,a0,63
  a2be56:	04000413          	li	s0,64
  a2be5a:	8c1d                	sub	s0,s0,a5
  a2be5c:	89aa                	mv	s3,a0
  a2be5e:	892e                	mv	s2,a1
  a2be60:	84b2                	mv	s1,a2
  a2be62:	8ab6                	mv	s5,a3
  a2be64:	00867363          	bgeu	a2,s0,a2be6a <hal_sfc_reg_write+0x1a>
  a2be68:	8432                	mv	s0,a2
  a2be6a:	86d6                	mv	a3,s5
  a2be6c:	8622                	mv	a2,s0
  a2be6e:	85ca                	mv	a1,s2
  a2be70:	854e                	mv	a0,s3
  a2be72:	db1ff0ef          	jal	ra,a2bc22 <hal_sfc_reg_write_once>
  a2be76:	8a2a                	mv	s4,a0
  a2be78:	e905                	bnez	a0,a2bea8 <hal_sfc_reg_write+0x58>
  a2be7a:	8c81                	sub	s1,s1,s0
  a2be7c:	c495                	beqz	s1,a2bea8 <hal_sfc_reg_write+0x58>
  a2be7e:	9922                	add	s2,s2,s0
  a2be80:	04000b13          	li	s6,64
  a2be84:	944e                	add	s0,s0,s3
  a2be86:	89a6                	mv	s3,s1
  a2be88:	009b7463          	bgeu	s6,s1,a2be90 <hal_sfc_reg_write+0x40>
  a2be8c:	04000993          	li	s3,64
  a2be90:	86d6                	mv	a3,s5
  a2be92:	864e                	mv	a2,s3
  a2be94:	85ca                	mv	a1,s2
  a2be96:	8522                	mv	a0,s0
  a2be98:	d8bff0ef          	jal	ra,a2bc22 <hal_sfc_reg_write_once>
  a2be9c:	e901                	bnez	a0,a2beac <hal_sfc_reg_write+0x5c>
  a2be9e:	413484b3          	sub	s1,s1,s3
  a2bea2:	994e                	add	s2,s2,s3
  a2bea4:	944e                	add	s0,s0,s3
  a2bea6:	f0e5                	bnez	s1,a2be86 <hal_sfc_reg_write+0x36>
  a2bea8:	8552                	mv	a0,s4
  a2beaa:	8084                	popret	{ra,s0-s6},32
  a2beac:	8a2a                	mv	s4,a0
  a2beae:	bfed                	j	a2bea8 <hal_sfc_reg_write+0x58>

00a2beb0 <hal_sfc_reg_erase>:
  a2beb0:	8238                	push	{ra,s0-s1},-48
  a2beb2:	c62e                	sw	a1,12(sp)
  a2beb4:	84aa                	mv	s1,a0
  a2beb6:	8432                	mv	s0,a2
  a2beb8:	d55ff0ef          	jal	ra,a2bc0c <hal_sfc_write_enable>
  a2bebc:	45b2                	lw	a1,12(sp)
  a2bebe:	e10d                	bnez	a0,a2bee0 <hal_sfc_reg_erase+0x30>
  a2bec0:	0848                	addi	a0,sp,20
  a2bec2:	ca2e                	sw	a1,20(sp)
  a2bec4:	cc02                	sw	zero,24(sp)
  a2bec6:	ce02                	sw	zero,28(sp)
  a2bec8:	2829                	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2beca:	8526                	mv	a0,s1
  a2becc:	c67ff0ef          	jal	ra,a2bb32 <hal_sfc_regs_set_cmd_addr>
  a2bed0:	00144613          	xori	a2,s0,1
  a2bed4:	4581                	li	a1,0
  a2bed6:	4501                	li	a0,0
  a2bed8:	2089                	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2beda:	20bd                	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2bedc:	c75ff0ef          	jal	ra,a2bb50 <hal_sfc_regs_wait_ready.constprop.3>
  a2bee0:	8234                	popret	{ra,s0-s1},48

00a2bee2 <hal_sfc_regs_set_opt>:
  a2bee2:	00a057b7          	lui	a5,0xa05
  a2bee6:	7987a603          	lw	a2,1944(a5) # a05798 <g_sfc_cmd_regs>
  a2beea:	411c                	lw	a5,0(a0)
  a2beec:	450c                	lw	a1,8(a0)
  a2beee:	4618                	lw	a4,8(a2)
  a2bef0:	0037d693          	srli	a3,a5,0x3
  a2bef4:	9e81                	uxtb	a3
  a2bef6:	f0077713          	andi	a4,a4,-256
  a2befa:	8f55                	or	a4,a4,a3
  a2befc:	4154                	lw	a3,4(a0)
  a2befe:	83ad                	srli	a5,a5,0xb
  a2bf00:	8b9d                	andi	a5,a5,7
  a2bf02:	899d                	andi	a1,a1,7
  a2bf04:	07c6                	slli	a5,a5,0x11
  a2bf06:	16fd                	addi	a3,a3,-1
  a2bf08:	03f6f693          	andi	a3,a3,63
  a2bf0c:	08b7a79b          	orshf	a5,a5,a1,sll,4
  a2bf10:	12d7a79b          	orshf	a5,a5,a3,sll,9
  a2bf14:	c618                	sw	a4,8(a2)
  a2bf16:	c21c                	sw	a5,0(a2)
  a2bf18:	8082                	ret

00a2bf1a <hal_sfc_regs_set_opt_attr>:
  a2bf1a:	00a057b7          	lui	a5,0xa05
  a2bf1e:	7987a703          	lw	a4,1944(a5) # a05798 <g_sfc_cmd_regs>
  a2bf22:	8905                	andi	a0,a0,1
  a2bf24:	8985                	andi	a1,a1,1
  a2bf26:	431c                	lw	a5,0(a4)
  a2bf28:	8a05                	andi	a2,a2,1
  a2bf2a:	eff7f793          	andi	a5,a5,-257
  a2bf2e:	10a7a79b          	orshf	a5,a5,a0,sll,8
  a2bf32:	f7f7f793          	andi	a5,a5,-129
  a2bf36:	0eb7a79b          	orshf	a5,a5,a1,sll,7
  a2bf3a:	9bdd                	andi	a5,a5,-9
  a2bf3c:	06c7a79b          	orshf	a5,a5,a2,sll,3
  a2bf40:	0037e793          	ori	a5,a5,3
  a2bf44:	c31c                	sw	a5,0(a4)
  a2bf46:	8082                	ret

00a2bf48 <hal_sfc_regs_wait_config>:
  a2bf48:	00a057b7          	lui	a5,0xa05
  a2bf4c:	7987a703          	lw	a4,1944(a5) # a05798 <g_sfc_cmd_regs>
  a2bf50:	431c                	lw	a5,0(a4)
  a2bf52:	8b85                	andi	a5,a5,1
  a2bf54:	fff5                	bnez	a5,a2bf50 <hal_sfc_regs_wait_config+0x8>
  a2bf56:	8082                	ret

00a2bf58 <hal_sfc_regs_set_bus_read>:
  a2bf58:	00a057b7          	lui	a5,0xa05
  a2bf5c:	7907a603          	lw	a2,1936(a5) # a05790 <g_sfc_bus_regs>
  a2bf60:	80000737          	lui	a4,0x80000
  a2bf64:	ffff 00ff 069f      	l.li	a3,0xffff00ff
  a2bf6a:	421c                	lw	a5,0(a2)
  a2bf6c:	8fd9                	or	a5,a5,a4
  a2bf6e:	00355713          	srli	a4,a0,0x3
  a2bf72:	9f01                	uxtb	a4
  a2bf74:	8ff5                	and	a5,a5,a3
  a2bf76:	10e7a79b          	orshf	a5,a5,a4,sll,8
  a2bf7a:	00b55693          	srli	a3,a0,0xb
  a2bf7e:	8a9d                	andi	a3,a3,7
  a2bf80:	812d                	srli	a0,a0,0xb
  a2bf82:	fc07f793          	andi	a5,a5,-64
  a2bf86:	03857713          	andi	a4,a0,56
  a2bf8a:	00d7e533          	or	a0,a5,a3
  a2bf8e:	8d59                	or	a0,a0,a4
  a2bf90:	c208                	sw	a0,0(a2)
  a2bf92:	8082                	ret

00a2bf94 <hal_sfc_regs_set_bus_write>:
  a2bf94:	00a057b7          	lui	a5,0xa05
  a2bf98:	7907a683          	lw	a3,1936(a5) # a05790 <g_sfc_bus_regs>
  a2bf9c:	00355713          	srli	a4,a0,0x3
  a2bfa0:	c03f ffff 061f      	l.li	a2,0xc03fffff
  a2bfa6:	429c                	lw	a5,0(a3)
  a2bfa8:	9f01                	uxtb	a4
  a2bfaa:	812d                	srli	a0,a0,0xb
  a2bfac:	8ff1                	and	a5,a5,a2
  a2bfae:	2ce7a79b          	orshf	a5,a5,a4,sll,22
  a2bfb2:	fff8 ffff 071f      	l.li	a4,0xfff8ffff
  a2bfb8:	8ff9                	and	a5,a5,a4
  a2bfba:	891d                	andi	a0,a0,7
  a2bfbc:	20a7a79b          	orshf	a5,a5,a0,sll,16
  a2bfc0:	40000737          	lui	a4,0x40000
  a2bfc4:	8fd9                	or	a5,a5,a4
  a2bfc6:	c29c                	sw	a5,0(a3)
  a2bfc8:	8082                	ret

00a2bfca <pmp_enable>:
  a2bfca:	8148                	push	{ra,s0-s2},-32
  a2bfcc:	004c                	addi	a1,sp,4
  a2bfce:	02000513          	li	a0,32
  a2bfd2:	c202                	sw	zero,4(sp)
  a2bfd4:	c402                	sw	zero,8(sp)
  a2bfd6:	c602                	sw	zero,12(sp)
  a2bfd8:	3dd010ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2bfdc:	4722                	lw	a4,8(sp)
  a2bfde:	00a34437          	lui	s0,0xa34
  a2bfe2:	00200937          	lui	s2,0x200
  a2bfe6:	974a                	add	a4,a4,s2
  a2bfe8:	ecc40493          	addi	s1,s0,-308 # a33ecc <g_region_attr>
  a2bfec:	004c                	addi	a1,sp,4
  a2bfee:	02100513          	li	a0,33
  a2bff2:	d0d8                	sw	a4,36(s1)
  a2bff4:	3c1010ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2bff8:	47a2                	lw	a5,8(sp)
  a2bffa:	4595                	li	a1,5
  a2bffc:	ecc40513          	addi	a0,s0,-308
  a2c000:	97ca                	add	a5,a5,s2
  a2c002:	d8dc                	sw	a5,52(s1)
  a2c004:	8dbfd0ef          	jal	ra,a298de <uapi_pmp_config>
  a2c008:	8144                	popret	{ra,s0-s2},32

00a2c00a <uart_port_register_hal_funcs>:
  a2c00a:	8028                	push	{ra,s0},-16
  a2c00c:	842a                	mv	s0,a0
  a2c00e:	917ff0ef          	jal	ra,a2b924 <hal_uart_v151_funcs_get>
  a2c012:	85aa                	mv	a1,a0
  a2c014:	8522                	mv	a0,s0
  a2c016:	8020                	pop	{ra,s0},16
  a2c018:	c82ff06f          	j	a2b49a <hal_uart_register_funcs>

00a2c01c <uart_port_set_clock_value>:
  a2c01c:	00a3 3f1c 079f      	l.li	a5,0xa33f1c
  a2c022:	c38c                	sw	a1,0(a5)
  a2c024:	8082                	ret

00a2c026 <uart_port_get_clock_value>:
  a2c026:	00a3 3f1c 079f      	l.li	a5,0xa33f1c
  a2c02c:	4388                	lw	a0,0(a5)
  a2c02e:	8082                	ret

00a2c030 <uart_port_config_pinmux>:
  a2c030:	020513bb          	bnei	a0,2,a2c03e <uart_port_config_pinmux+0xe>
  a2c034:	4400d7b7          	lui	a5,0x4400d
  a2c038:	4705                	li	a4,1
  a2c03a:	db98                	sw	a4,48(a5)
  a2c03c:	dbd8                	sw	a4,52(a5)
  a2c03e:	8082                	ret

00a2c040 <uart_port_register_irq>:
  a2c040:	8082                	ret

00a2c042 <uart_port_unregister_irq>:
  a2c042:	8082                	ret

00a2c044 <print_str>:
  a2c044:	715d                	addi	sp,sp,-80
  a2c046:	d606                	sw	ra,44(sp)
  a2c048:	d422                	sw	s0,40(sp)
  a2c04a:	da2e                	sw	a1,52(sp)
  a2c04c:	dc32                	sw	a2,56(sp)
  a2c04e:	de36                	sw	a3,60(sp)
  a2c050:	c0ba                	sw	a4,64(sp)
  a2c052:	c2be                	sw	a5,68(sp)
  a2c054:	c4c2                	sw	a6,72(sp)
  a2c056:	c6c6                	sw	a7,76(sp)
  a2c058:	c131                	beqz	a0,a2c09c <print_str+0x58>
  a2c05a:	c62a                	sw	a0,12(sp)
  a2c05c:	cb1fc0ef          	jal	ra,a28d0c <strlen>
  a2c060:	46b2                	lw	a3,12(sp)
  a2c062:	cd0d                	beqz	a0,a2c09c <print_str+0x58>
  a2c064:	00a05437          	lui	s0,0xa05
  a2c068:	1858                	addi	a4,sp,52
  a2c06a:	07f00613          	li	a2,127
  a2c06e:	08000593          	li	a1,128
  a2c072:	7a040513          	addi	a0,s0,1952 # a057a0 <str_buf.2970>
  a2c076:	ce3a                	sw	a4,28(sp)
  a2c078:	2b6050ef          	jal	ra,a3132e <vsnprintf_s>
  a2c07c:	862a                	mv	a2,a0
  a2c07e:	7a040793          	addi	a5,s0,1952
  a2c082:	ff05123b          	bnei	a0,-1,a2c08a <print_str+0x46>
  a2c086:	2398                	lbu	a4,0(a5)
  a2c088:	cb11                	beqz	a4,a2c09c <print_str+0x58>
  a2c08a:	97b2                	add	a5,a5,a2
  a2c08c:	00078023          	sb	zero,0(a5) # 4400d000 <_gp_+0x435d8844>
  a2c090:	4681                	li	a3,0
  a2c092:	7a040593          	addi	a1,s0,1952
  a2c096:	4501                	li	a0,0
  a2c098:	dc6fe0ef          	jal	ra,a2a65e <uapi_uart_write>
  a2c09c:	50b2                	lw	ra,44(sp)
  a2c09e:	5422                	lw	s0,40(sp)
  a2c0a0:	6161                	addi	sp,sp,80
  a2c0a2:	8082                	ret

00a2c0a4 <uart_porting_lock>:
  a2c0a4:	831fd06f          	j	a298d4 <osal_irq_lock>

00a2c0a8 <uart_porting_unlock>:
  a2c0a8:	852e                	mv	a0,a1
  a2c0aa:	833fd06f          	j	a298dc <osal_irq_restore>

00a2c0ae <hal_systick_get_count>:
  a2c0ae:	400057b7          	lui	a5,0x40005
  a2c0b2:	4fd8                	lw	a4,28(a5)
  a2c0b4:	4f98                	lw	a4,24(a5)
  a2c0b6:	4fc8                	lw	a0,28(a5)
  a2c0b8:	4f8c                	lw	a1,24(a5)
  a2c0ba:	9da1                	uxth	a1
  a2c0bc:	8082                	ret

00a2c0be <systick_clock_get>:
  a2c0be:	00a3 3f20 079f      	l.li	a5,0xa33f20
  a2c0c4:	4388                	lw	a0,0(a5)
  a2c0c6:	8082                	ret

00a2c0c8 <tcxo_porting_base_addr_get>:
  a2c0c8:	4400 04c0 051f      	l.li	a0,0x440004c0
  a2c0ce:	8082                	ret

00a2c0d0 <tcxo_porting_ticks_per_usec_get>:
  a2c0d0:	00a3 3f24 079f      	l.li	a5,0xa33f24
  a2c0d6:	4388                	lw	a0,0(a5)
  a2c0d8:	8082                	ret

00a2c0da <tcxo_porting_ticks_per_usec_set>:
  a2c0da:	00a3 3f24 079f      	l.li	a5,0xa33f24
  a2c0e0:	c388                	sw	a0,0(a5)
  a2c0e2:	8082                	ret

00a2c0e4 <watchdog_port_register_hal_funcs>:
  a2c0e4:	8018                	push	{ra},-16
  a2c0e6:	9ebff0ef          	jal	ra,a2bad0 <hal_watchdog_v151_funcs_get>
  a2c0ea:	8010                	pop	{ra},16
  a2c0ec:	841ff06f          	j	a2b92c <hal_watchdog_register_funcs>

00a2c0f0 <watchdog_port_unregister_hal_funcs>:
  a2c0f0:	853ff06f          	j	a2b942 <hal_watchdog_unregister_funcs>

00a2c0f4 <watchdog_port_register_irq>:
  a2c0f4:	8082                	ret

00a2c0f6 <watchdog_port_set_clock>:
  a2c0f6:	8028                	push	{ra,s0},-16
  a2c0f8:	842a                	mv	s0,a0
  a2c0fa:	fdafd0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2c0fe:	00a3 3f2c 079f      	l.li	a5,0xa33f2c
  a2c104:	c380                	sw	s0,0(a5)
  a2c106:	8020                	pop	{ra,s0},16
  a2c108:	fd4fd06f          	j	a298dc <osal_irq_restore>

00a2c10c <watchdog_port_get_clock>:
  a2c10c:	00a3 3f2c 079f      	l.li	a5,0xa33f2c
  a2c112:	4388                	lw	a0,0(a5)
  a2c114:	8082                	ret

00a2c116 <pmp_port_register_hal_funcs>:
  a2c116:	8018                	push	{ra},-16
  a2c118:	a43fe0ef          	jal	ra,a2ab5a <hal_pmp_riscv31_funcs_get>
  a2c11c:	8010                	pop	{ra},16
  a2c11e:	9d1fe06f          	j	a2aaee <hal_pmp_register_funcs>

00a2c122 <sfc_port_get_flash_spi_infos>:
  a2c122:	00a3 2774 051f      	l.li	a0,0xa32774
  a2c128:	8082                	ret

00a2c12a <sfc_port_get_flash_num>:
  a2c12a:	4505                	li	a0,1
  a2c12c:	8082                	ret

00a2c12e <sfc_port_get_unknown_flash_info>:
  a2c12e:	00a3 2790 051f      	l.li	a0,0xa32790
  a2c134:	8082                	ret

00a2c136 <sfc_port_get_sfc_start_addr>:
  a2c136:	00200537          	lui	a0,0x200
  a2c13a:	8082                	ret

00a2c13c <sfc_port_get_sfc_end_addr>:
  a2c13c:	009f ffff 051f      	l.li	a0,0x9fffff
  a2c142:	8082                	ret

00a2c144 <sfc_port_get_sfc_global_conf_base_addr>:
  a2c144:	4800 0100 051f      	l.li	a0,0x48000100
  a2c14a:	8082                	ret

00a2c14c <sfc_port_get_sfc_bus_regs_base_addr>:
  a2c14c:	4800 0200 051f      	l.li	a0,0x48000200
  a2c152:	8082                	ret

00a2c154 <sfc_port_get_sfc_bus_dma_regs_base_addr>:
  a2c154:	4800 0240 051f      	l.li	a0,0x48000240
  a2c15a:	8082                	ret

00a2c15c <sfc_port_get_sfc_cmd_regs_base_addr>:
  a2c15c:	4800 0300 051f      	l.li	a0,0x48000300
  a2c162:	8082                	ret

00a2c164 <sfc_port_get_sfc_cmd_databuf_base_addr>:
  a2c164:	4800 0400 051f      	l.li	a0,0x48000400
  a2c16a:	8082                	ret

00a2c16c <sfc_port_get_delay_once_time>:
  a2c16c:	00a3 3f30 079f      	l.li	a5,0xa33f30
  a2c172:	4388                	lw	a0,0(a5)
  a2c174:	8082                	ret

00a2c176 <sfc_port_get_delay_times>:
  a2c176:	00a3 3f34 079f      	l.li	a5,0xa33f34
  a2c17c:	4388                	lw	a0,0(a5)
  a2c17e:	8082                	ret

00a2c180 <sfc_port_lock_init>:
  a2c180:	8082                	ret

00a2c182 <sfc_port_lock>:
  a2c182:	4501                	li	a0,0
  a2c184:	8082                	ret

00a2c186 <sfc_port_unlock>:
  a2c186:	8082                	ret

00a2c188 <hal_sfc_regs_wait_ready>:
  a2c188:	8168                	push	{ra,s0-s4},-48
  a2c18a:	842a                	mv	s0,a0
  a2c18c:	febff0ef          	jal	ra,a2c176 <sfc_port_get_delay_times>
  a2c190:	892a                	mv	s2,a0
  a2c192:	fdbff0ef          	jal	ra,a2c16c <sfc_port_get_delay_once_time>
  a2c196:	89aa                	mv	s3,a0
  a2c198:	02800793          	li	a5,40
  a2c19c:	4505                	li	a0,1
  a2c19e:	c602                	sw	zero,12(sp)
  a2c1a0:	c23e                	sw	a5,4(sp)
  a2c1a2:	c42a                	sw	a0,8(sp)
  a2c1a4:	00851433          	sll	s0,a0,s0
  a2c1a8:	4481                	li	s1,0
  a2c1aa:	00a05a37          	lui	s4,0xa05
  a2c1ae:	0048                	addi	a0,sp,4
  a2c1b0:	d33ff0ef          	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2c1b4:	4601                	li	a2,0
  a2c1b6:	4585                	li	a1,1
  a2c1b8:	4505                	li	a0,1
  a2c1ba:	d61ff0ef          	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2c1be:	d8bff0ef          	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2c1c2:	794a2783          	lw	a5,1940(s4) # a05794 <g_sfc_cmd_databuf>
  a2c1c6:	439c                	lw	a5,0(a5)
  a2c1c8:	8fe1                	and	a5,a5,s0
  a2c1ca:	cf91                	beqz	a5,a2c1e6 <hal_sfc_regs_wait_ready+0x5e>
  a2c1cc:	854e                	mv	a0,s3
  a2c1ce:	8c6fe0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2c1d2:	00148793          	addi	a5,s1,1
  a2c1d6:	0124e663          	bltu	s1,s2,a2c1e2 <hal_sfc_regs_wait_ready+0x5a>
  a2c1da:	8000 1344 051f      	l.li	a0,0x80001344
  a2c1e0:	8164                	popret	{ra,s0-s4},48
  a2c1e2:	84be                	mv	s1,a5
  a2c1e4:	b7e9                	j	a2c1ae <hal_sfc_regs_wait_ready+0x26>
  a2c1e6:	4501                	li	a0,0
  a2c1e8:	bfe5                	j	a2c1e0 <hal_sfc_regs_wait_ready+0x58>

00a2c1ea <sfc_port_write_sr>:
  a2c1ea:	8148                	push	{ra,s0-s2},-32
  a2c1ec:	892a                	mv	s2,a0
  a2c1ee:	4501                	li	a0,0
  a2c1f0:	842e                	mv	s0,a1
  a2c1f2:	84b2                	mv	s1,a2
  a2c1f4:	f95ff0ef          	jal	ra,a2c188 <hal_sfc_regs_wait_ready>
  a2c1f8:	4792                	lw	a5,4(sp)
  a2c1fa:	4719                	li	a4,6
  a2c1fc:	9be1                	andi	a5,a5,-8
  a2c1fe:	0017e793          	ori	a5,a5,1
  a2c202:	c23e                	sw	a5,4(sp)
  a2c204:	00090463          	beqz	s2,a2c20c <sfc_port_write_sr+0x22>
  a2c208:	05000713          	li	a4,80
  a2c20c:	4792                	lw	a5,4(sp)
  a2c20e:	0048                	addi	a0,sp,4
  a2c210:	c402                	sw	zero,8(sp)
  a2c212:	8077f793          	andi	a5,a5,-2041
  a2c216:	06e7a79b          	orshf	a5,a5,a4,sll,3
  a2c21a:	7ff7f793          	andi	a5,a5,2047
  a2c21e:	c23e                	sw	a5,4(sp)
  a2c220:	c602                	sw	zero,12(sp)
  a2c222:	cc1ff0ef          	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2c226:	4601                	li	a2,0
  a2c228:	4581                	li	a1,0
  a2c22a:	4501                	li	a0,0
  a2c22c:	cefff0ef          	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2c230:	d19ff0ef          	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2c234:	4785                	li	a5,1
  a2c236:	c43e                	sw	a5,8(sp)
  a2c238:	00a057b7          	lui	a5,0xa05
  a2c23c:	7947a783          	lw	a5,1940(a5) # a05794 <g_sfc_cmd_databuf>
  a2c240:	040e                	slli	s0,s0,0x3
  a2c242:	00146413          	ori	s0,s0,1
  a2c246:	c222                	sw	s0,4(sp)
  a2c248:	c602                	sw	zero,12(sp)
  a2c24a:	c384                	sw	s1,0(a5)
  a2c24c:	0048                	addi	a0,sp,4
  a2c24e:	c95ff0ef          	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2c252:	4601                	li	a2,0
  a2c254:	4585                	li	a1,1
  a2c256:	4501                	li	a0,0
  a2c258:	cc3ff0ef          	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2c25c:	cedff0ef          	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2c260:	8144                	popret	{ra,s0-s2},32

00a2c262 <sfc_port_read_sr>:
  a2c262:	8118                	push	{ra},-32
  a2c264:	0ff57793          	andi	a5,a0,255
  a2c268:	4512                	lw	a0,4(sp)
  a2c26a:	c602                	sw	zero,12(sp)
  a2c26c:	80757513          	andi	a0,a0,-2041
  a2c270:	06f5251b          	orshf	a0,a0,a5,sll,3
  a2c274:	ffff c7ff 079f      	l.li	a5,0xffffc7ff
  a2c27a:	8d7d                	and	a0,a0,a5
  a2c27c:	c22a                	sw	a0,4(sp)
  a2c27e:	4785                	li	a5,1
  a2c280:	4501                	li	a0,0
  a2c282:	c43e                	sw	a5,8(sp)
  a2c284:	f05ff0ef          	jal	ra,a2c188 <hal_sfc_regs_wait_ready>
  a2c288:	0048                	addi	a0,sp,4
  a2c28a:	c59ff0ef          	jal	ra,a2bee2 <hal_sfc_regs_set_opt>
  a2c28e:	4601                	li	a2,0
  a2c290:	4585                	li	a1,1
  a2c292:	4505                	li	a0,1
  a2c294:	c87ff0ef          	jal	ra,a2bf1a <hal_sfc_regs_set_opt_attr>
  a2c298:	cb1ff0ef          	jal	ra,a2bf48 <hal_sfc_regs_wait_config>
  a2c29c:	00a057b7          	lui	a5,0xa05
  a2c2a0:	7947a783          	lw	a5,1940(a5) # a05794 <g_sfc_cmd_databuf>
  a2c2a4:	4388                	lw	a0,0(a5)
  a2c2a6:	8114                	popret	{ra},32

00a2c2a8 <sfc_port_write_lock>:
  a2c2a8:	000207b7          	lui	a5,0x20
  a2c2ac:	02f56563          	bltu	a0,a5,a2c2d6 <sfc_port_write_lock+0x2e>
  a2c2b0:	004007b7          	lui	a5,0x400
  a2c2b4:	02b7e163          	bltu	a5,a1,a2c2d6 <sfc_port_write_lock+0x2e>
  a2c2b8:	8018                	push	{ra},-16
  a2c2ba:	02800613          	li	a2,40
  a2c2be:	4585                	li	a1,1
  a2c2c0:	4505                	li	a0,1
  a2c2c2:	f29ff0ef          	jal	ra,a2c1ea <sfc_port_write_sr>
  a2c2c6:	4609                	li	a2,2
  a2c2c8:	03100593          	li	a1,49
  a2c2cc:	4505                	li	a0,1
  a2c2ce:	f1dff0ef          	jal	ra,a2c1ea <sfc_port_write_sr>
  a2c2d2:	4501                	li	a0,0
  a2c2d4:	8014                	popret	{ra},16
  a2c2d6:	4501                	li	a0,0
  a2c2d8:	8082                	ret

00a2c2da <sfc_port_write_unlock>:
  a2c2da:	8018                	push	{ra},-16
  a2c2dc:	4671                	li	a2,28
  a2c2de:	4585                	li	a1,1
  a2c2e0:	4505                	li	a0,1
  a2c2e2:	f09ff0ef          	jal	ra,a2c1ea <sfc_port_write_sr>
  a2c2e6:	4609                	li	a2,2
  a2c2e8:	03100593          	li	a1,49
  a2c2ec:	4505                	li	a0,1
  a2c2ee:	8010                	pop	{ra},16
  a2c2f0:	efbff06f          	j	a2c1ea <sfc_port_write_sr>

00a2c2f4 <sfc_port_fix_sr>:
  a2c2f4:	8118                	push	{ra},-32
  a2c2f6:	0068                	addi	a0,sp,12
  a2c2f8:	c602                	sw	zero,12(sp)
  a2c2fa:	9a5ff0ef          	jal	ra,a2bc9e <hal_sfc_get_flash_id>
  a2c2fe:	e901                	bnez	a0,a2c30e <sfc_port_fix_sr+0x1a>
  a2c300:	4732                	lw	a4,12(sp)
  a2c302:	0016 40c8 079f      	l.li	a5,0x1640c8
  a2c308:	00f71463          	bne	a4,a5,a2c310 <sfc_port_fix_sr+0x1c>
  a2c30c:	2079                	jal	ra,a2c39a <sfc_port_fix_sr_gd25q32>
  a2c30e:	8114                	popret	{ra},32
  a2c310:	80000537          	lui	a0,0x80000
  a2c314:	0509                	addi	a0,a0,2 # 80000002 <_gp_+0x7f5cb846>
  a2c316:	bfe5                	j	a2c30e <sfc_port_fix_sr+0x1a>

00a2c318 <sfc_port_gd25q32_read_sr>:
  a2c318:	8078                	push	{ra,s0-s5},-32
  a2c31a:	892a                	mv	s2,a0
  a2c31c:	448d                	li	s1,3
  a2c31e:	4995                	li	s3,5
  a2c320:	03500a13          	li	s4,53
  a2c324:	00a32ab7          	lui	s5,0xa32
  a2c328:	854a                	mv	a0,s2
  a2c32a:	f39ff0ef          	jal	ra,a2c262 <sfc_port_read_sr>
  a2c32e:	0ff57413          	andi	s0,a0,255
  a2c332:	03391f63          	bne	s2,s3,a2c370 <sfc_port_gd25q32_read_sr+0x58>
  a2c336:	09c47413          	andi	s0,s0,156
  a2c33a:	1411                	addi	s0,s0,-28
  a2c33c:	00143413          	seqz	s0,s0
  a2c340:	4585                	li	a1,1
  a2c342:	46f1                	li	a3,28
  a2c344:	e419                	bnez	s0,a2c352 <sfc_port_gd25q32_read_sr+0x3a>
  a2c346:	0ff57613          	andi	a2,a0,255
  a2c34a:	7d4a8513          	addi	a0,s5,2004 # a327d4 <g_flash_spi_unknown_info+0x44>
  a2c34e:	cf7ff0ef          	jal	ra,a2c044 <print_str>
  a2c352:	ec3fd0ef          	jal	ra,a2a214 <uapi_tcxo_get_count>
  a2c356:	06400613          	li	a2,100
  a2c35a:	4681                	li	a3,0
  a2c35c:	a44fc0ef          	jal	ra,a285a0 <__umoddi3>
  a2c360:	0505                	addi	a0,a0,1
  a2c362:	f33fd0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2c366:	c805                	beqz	s0,a2c396 <sfc_port_gd25q32_read_sr+0x7e>
  a2c368:	14fd                	addi	s1,s1,-1
  a2c36a:	fcdd                	bnez	s1,a2c328 <sfc_port_gd25q32_read_sr+0x10>
  a2c36c:	4501                	li	a0,0
  a2c36e:	8074                	popret	{ra,s0-s5},32
  a2c370:	01491a63          	bne	s2,s4,a2c384 <sfc_port_gd25q32_read_sr+0x6c>
  a2c374:	04347413          	andi	s0,s0,67
  a2c378:	1479                	addi	s0,s0,-2
  a2c37a:	00143413          	seqz	s0,s0
  a2c37e:	4589                	li	a1,2
  a2c380:	4689                	li	a3,2
  a2c382:	b7c9                	j	a2c344 <sfc_port_gd25q32_read_sr+0x2c>
  a2c384:	06147413          	andi	s0,s0,97
  a2c388:	1401                	addi	s0,s0,-32
  a2c38a:	00143413          	seqz	s0,s0
  a2c38e:	458d                	li	a1,3
  a2c390:	02000693          	li	a3,32
  a2c394:	bf45                	j	a2c344 <sfc_port_gd25q32_read_sr+0x2c>
  a2c396:	557d                	li	a0,-1
  a2c398:	bfd9                	j	a2c36e <sfc_port_gd25q32_read_sr+0x56>

00a2c39a <sfc_port_fix_sr_gd25q32>:
  a2c39a:	8068                	push	{ra,s0-s4},-32
  a2c39c:	4481                	li	s1,0
  a2c39e:	498d                	li	s3,3
  a2c3a0:	4515                	li	a0,5
  a2c3a2:	f77ff0ef          	jal	ra,a2c318 <sfc_port_gd25q32_read_sr>
  a2c3a6:	8a2a                	mv	s4,a0
  a2c3a8:	03500513          	li	a0,53
  a2c3ac:	f6dff0ef          	jal	ra,a2c318 <sfc_port_gd25q32_read_sr>
  a2c3b0:	892a                	mv	s2,a0
  a2c3b2:	4555                	li	a0,21
  a2c3b4:	f65ff0ef          	jal	ra,a2c318 <sfc_port_gd25q32_read_sr>
  a2c3b8:	842a                	mv	s0,a0
  a2c3ba:	040a1763          	bnez	s4,a2c408 <sfc_port_fix_sr_gd25q32+0x6e>
  a2c3be:	02090863          	beqz	s2,a2c3ee <sfc_port_fix_sr_gd25q32+0x54>
  a2c3c2:	4609                	li	a2,2
  a2c3c4:	03100593          	li	a1,49
  a2c3c8:	4501                	li	a0,0
  a2c3ca:	e21ff0ef          	jal	ra,a2c1ea <sfc_port_write_sr>
  a2c3ce:	c419                	beqz	s0,a2c3dc <sfc_port_fix_sr_gd25q32+0x42>
  a2c3d0:	02000613          	li	a2,32
  a2c3d4:	45c5                	li	a1,17
  a2c3d6:	4501                	li	a0,0
  a2c3d8:	e13ff0ef          	jal	ra,a2c1ea <sfc_port_write_sr>
  a2c3dc:	03c00513          	li	a0,60
  a2c3e0:	0485                	addi	s1,s1,1
  a2c3e2:	e63fd0ef          	jal	ra,a2a244 <uapi_tcxo_delay_ms>
  a2c3e6:	fb349de3          	bne	s1,s3,a2c3a0 <sfc_port_fix_sr_gd25q32+0x6>
  a2c3ea:	547d                	li	s0,-1
  a2c3ec:	a801                	j	a2c3fc <sfc_port_fix_sr_gd25q32+0x62>
  a2c3ee:	f16d                	bnez	a0,a2c3d0 <sfc_port_fix_sr_gd25q32+0x36>
  a2c3f0:	e881                	bnez	s1,a2c400 <sfc_port_fix_sr_gd25q32+0x66>
  a2c3f2:	00a3 27ac 051f      	l.li	a0,0xa327ac
  a2c3f8:	c4dff0ef          	jal	ra,a2c044 <print_str>
  a2c3fc:	8522                	mv	a0,s0
  a2c3fe:	8064                	popret	{ra,s0-s4},32
  a2c400:	00a3 27c4 051f      	l.li	a0,0xa327c4
  a2c406:	bfcd                	j	a2c3f8 <sfc_port_fix_sr_gd25q32+0x5e>
  a2c408:	4671                	li	a2,28
  a2c40a:	4585                	li	a1,1
  a2c40c:	4501                	li	a0,0
  a2c40e:	dddff0ef          	jal	ra,a2c1ea <sfc_port_write_sr>
  a2c412:	fa090ee3          	beqz	s2,a2c3ce <sfc_port_fix_sr_gd25q32+0x34>
  a2c416:	b775                	j	a2c3c2 <sfc_port_fix_sr_gd25q32+0x28>

00a2c418 <build_flash_ctrl>:
  a2c418:	455c                	lw	a5,12(a0)
  a2c41a:	04b7859b          	addshf	a1,a5,a1,sll,2
  a2c41e:	4194                	lw	a3,0(a1)
  a2c420:	491c                	lw	a5,16(a0)
  a2c422:	04c7861b          	addshf	a2,a5,a2,sll,2
  a2c426:	0076f793          	andi	a5,a3,7
  a2c42a:	4218                	lw	a4,0(a2)
  a2c42c:	01079bbb          	bnei	a5,1,a2c45a <build_flash_ctrl+0x42>
  a2c430:	8b1d                	andi	a4,a4,7
  a2c432:	02f71463          	bne	a4,a5,a2c45a <build_flash_ctrl+0x42>
  a2c436:	00a0 5820 079f      	l.li	a5,0xa05820
  a2c43c:	c3d4                	sw	a3,4(a5)
  a2c43e:	4214                	lw	a3,0(a2)
  a2c440:	c7d4                	sw	a3,12(a5)
  a2c442:	4954                	lw	a3,20(a0)
  a2c444:	cb94                	sw	a3,16(a5)
  a2c446:	4154                	lw	a3,4(a0)
  a2c448:	c394                	sw	a3,0(a5)
  a2c44a:	4514                	lw	a3,8(a0)
  a2c44c:	00d77b63          	bgeu	a4,a3,a2c462 <build_flash_ctrl+0x4a>
  a2c450:	4d18                	lw	a4,24(a0)
  a2c452:	c794                	sw	a3,8(a5)
  a2c454:	4501                	li	a0,0
  a2c456:	cbd8                	sw	a4,20(a5)
  a2c458:	8082                	ret
  a2c45a:	8000 1346 051f      	l.li	a0,0x80001346
  a2c460:	8082                	ret
  a2c462:	8000 1349 051f      	l.li	a0,0x80001349
  a2c468:	8082                	ret

00a2c46a <check_opt_param>:
  a2c46a:	00a0 5838 079f      	l.li	a5,0xa05838
  a2c470:	239c                	lbu	a5,0(a5)
  a2c472:	cf99                	beqz	a5,a2c490 <check_opt_param+0x26>
  a2c474:	00a0 5820 071f      	l.li	a4,0xa05820
  a2c47a:	4318                	lw	a4,0(a4)
  a2c47c:	00b507b3          	add	a5,a0,a1
  a2c480:	00f76c63          	bltu	a4,a5,a2c498 <check_opt_param+0x2e>
  a2c484:	00f57a63          	bgeu	a0,a5,a2c498 <check_opt_param+0x2e>
  a2c488:	00b7e863          	bltu	a5,a1,a2c498 <check_opt_param+0x2e>
  a2c48c:	4501                	li	a0,0
  a2c48e:	8082                	ret
  a2c490:	8000 1340 051f      	l.li	a0,0x80001340
  a2c496:	8082                	ret
  a2c498:	80000537          	lui	a0,0x80000
  a2c49c:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2c49e:	8082                	ret

00a2c4a0 <check_init_param>:
  a2c4a0:	8078                	push	{ra,s0-s5},-32
  a2c4a2:	00452a83          	lw	s5,4(a0)
  a2c4a6:	00a06937          	lui	s2,0xa06
  a2c4aa:	89aa                	mv	s3,a0
  a2c4ac:	82092403          	lw	s0,-2016(s2) # a05820 <g_flash_ctrl>
  a2c4b0:	00852a03          	lw	s4,8(a0)
  a2c4b4:	c83ff0ef          	jal	ra,a2c136 <sfc_port_get_sfc_start_addr>
  a2c4b8:	00aaf663          	bgeu	s5,a0,a2c4c4 <check_init_param+0x24>
  a2c4bc:	8000 1342 051f      	l.li	a0,0x80001342
  a2c4c2:	8074                	popret	{ra,s0-s5},32
  a2c4c4:	82090913          	addi	s2,s2,-2016
  a2c4c8:	008a7363          	bgeu	s4,s0,a2c4ce <check_init_param+0x2e>
  a2c4cc:	8452                	mv	s0,s4
  a2c4ce:	0049a483          	lw	s1,4(s3)
  a2c4d2:	14fd                	addi	s1,s1,-1
  a2c4d4:	94a2                	add	s1,s1,s0
  a2c4d6:	c67ff0ef          	jal	ra,a2c13c <sfc_port_get_sfc_end_addr>
  a2c4da:	fe9561e3          	bltu	a0,s1,a2c4bc <check_init_param+0x1c>
  a2c4de:	00892023          	sw	s0,0(s2)
  a2c4e2:	4501                	li	a0,0
  a2c4e4:	bff9                	j	a2c4c2 <check_init_param+0x22>

00a2c4e6 <uapi_sfc_init>:
  a2c4e6:	00a067b7          	lui	a5,0xa06
  a2c4ea:	8387c703          	lbu	a4,-1992(a5) # a05838 <g_sfc_inited>
  a2c4ee:	ef59                	bnez	a4,a2c58c <uapi_sfc_init+0xa6>
  a2c4f0:	8258                	push	{ra,s0-s3},-64
  a2c4f2:	842a                	mv	s0,a0
  a2c4f4:	83878913          	addi	s2,a5,-1992
  a2c4f8:	c89ff0ef          	jal	ra,a2c180 <sfc_port_lock_init>
  a2c4fc:	0868                	addi	a0,sp,28
  a2c4fe:	fa0ff0ef          	jal	ra,a2bc9e <hal_sfc_get_flash_id>
  a2c502:	e151                	bnez	a0,a2c586 <uapi_sfc_init+0xa0>
  a2c504:	200c                	lbu	a1,0(s0)
  a2c506:	3010                	lbu	a2,1(s0)
  a2c508:	49f2                	lw	s3,28(sp)
  a2c50a:	c62e                	sw	a1,12(sp)
  a2c50c:	c432                	sw	a2,8(sp)
  a2c50e:	c15ff0ef          	jal	ra,a2c122 <sfc_port_get_flash_spi_infos>
  a2c512:	84aa                	mv	s1,a0
  a2c514:	c17ff0ef          	jal	ra,a2c12a <sfc_port_get_flash_num>
  a2c518:	4622                	lw	a2,8(sp)
  a2c51a:	45b2                	lw	a1,12(sp)
  a2c51c:	86aa                	mv	a3,a0
  a2c51e:	87a6                	mv	a5,s1
  a2c520:	4701                	li	a4,0
  a2c522:	04e69963          	bne	a3,a4,a2c574 <uapi_sfc_init+0x8e>
  a2c526:	c09ff0ef          	jal	ra,a2c12e <sfc_port_get_unknown_flash_info>
  a2c52a:	4605                	li	a2,1
  a2c52c:	4581                	li	a1,0
  a2c52e:	eebff0ef          	jal	ra,a2c418 <build_flash_ctrl>
  a2c532:	e931                	bnez	a0,a2c586 <uapi_sfc_init+0xa0>
  a2c534:	00a0 5839 079f      	l.li	a5,0xa05839
  a2c53a:	4705                	li	a4,1
  a2c53c:	a398                	sb	a4,0(a5)
  a2c53e:	8522                	mv	a0,s0
  a2c540:	f61ff0ef          	jal	ra,a2c4a0 <check_init_param>
  a2c544:	e129                	bnez	a0,a2c586 <uapi_sfc_init+0xa0>
  a2c546:	00a06537          	lui	a0,0xa06
  a2c54a:	82052603          	lw	a2,-2016(a0) # a05820 <g_flash_ctrl>
  a2c54e:	404c                	lw	a1,4(s0)
  a2c550:	82050513          	addi	a0,a0,-2016
  a2c554:	f8eff0ef          	jal	ra,a2bce2 <hal_sfc_init>
  a2c558:	e51d                	bnez	a0,a2c586 <uapi_sfc_init+0xa0>
  a2c55a:	4785                	li	a5,1
  a2c55c:	00f90023          	sb	a5,0(s2)
  a2c560:	00a0 5839 079f      	l.li	a5,0xa05839
  a2c566:	239c                	lbu	a5,0(a5)
  a2c568:	4501                	li	a0,0
  a2c56a:	cf91                	beqz	a5,a2c586 <uapi_sfc_init+0xa0>
  a2c56c:	8000 1341 051f      	l.li	a0,0x80001341
  a2c572:	a811                	j	a2c586 <uapi_sfc_init+0xa0>
  a2c574:	853e                	mv	a0,a5
  a2c576:	07f1                	addi	a5,a5,28
  a2c578:	fe47a803          	lw	a6,-28(a5)
  a2c57c:	01099663          	bne	s3,a6,a2c588 <uapi_sfc_init+0xa2>
  a2c580:	e99ff0ef          	jal	ra,a2c418 <build_flash_ctrl>
  a2c584:	dd4d                	beqz	a0,a2c53e <uapi_sfc_init+0x58>
  a2c586:	8254                	popret	{ra,s0-s3},64
  a2c588:	0705                	addi	a4,a4,1 # 40000001 <_gp_+0x3f5cb845>
  a2c58a:	bf61                	j	a2c522 <uapi_sfc_init+0x3c>
  a2c58c:	4501                	li	a0,0
  a2c58e:	8082                	ret

00a2c590 <uapi_sfc_reg_read>:
  a2c590:	8188                	push	{ra,s0-s6},-48
  a2c592:	8a2e                	mv	s4,a1
  a2c594:	85b2                	mv	a1,a2
  a2c596:	892a                	mv	s2,a0
  a2c598:	84b2                	mv	s1,a2
  a2c59a:	ed1ff0ef          	jal	ra,a2c46a <check_opt_param>
  a2c59e:	e13d                	bnez	a0,a2c604 <uapi_sfc_reg_read+0x74>
  a2c5a0:	be3ff0ef          	jal	ra,a2c182 <sfc_port_lock>
  a2c5a4:	00397b13          	andi	s6,s2,3
  a2c5a8:	8aaa                	mv	s5,a0
  a2c5aa:	060b0263          	beqz	s6,a2c60e <uapi_sfc_reg_read+0x7e>
  a2c5ae:	4791                	li	a5,4
  a2c5b0:	416787b3          	sub	a5,a5,s6
  a2c5b4:	41690933          	sub	s2,s2,s6
  a2c5b8:	0ff4f993          	andi	s3,s1,255
  a2c5bc:	00f4e763          	bltu	s1,a5,a2c5ca <uapi_sfc_reg_read+0x3a>
  a2c5c0:	4991                	li	s3,4
  a2c5c2:	416989b3          	sub	s3,s3,s6
  a2c5c6:	0ff9f993          	andi	s3,s3,255
  a2c5ca:	00a0 5820 079f      	l.li	a5,0xa05820
  a2c5d0:	43d4                	lw	a3,4(a5)
  a2c5d2:	4611                	li	a2,4
  a2c5d4:	006c                	addi	a1,sp,12
  a2c5d6:	854a                	mv	a0,s2
  a2c5d8:	c602                	sw	zero,12(sp)
  a2c5da:	feaff0ef          	jal	ra,a2bdc4 <hal_sfc_reg_read>
  a2c5de:	842a                	mv	s0,a0
  a2c5e0:	ed11                	bnez	a0,a2c5fc <uapi_sfc_reg_read+0x6c>
  a2c5e2:	007c                	addi	a5,sp,12
  a2c5e4:	86ce                	mv	a3,s3
  a2c5e6:	01678633          	add	a2,a5,s6
  a2c5ea:	85ce                	mv	a1,s3
  a2c5ec:	8552                	mv	a0,s4
  a2c5ee:	382040ef          	jal	ra,a30970 <memcpy_s>
  a2c5f2:	c911                	beqz	a0,a2c606 <uapi_sfc_reg_read+0x76>
  a2c5f4:	80000537          	lui	a0,0x80000
  a2c5f8:	00450413          	addi	s0,a0,4 # 80000004 <_gp_+0x7f5cb848>
  a2c5fc:	8556                	mv	a0,s5
  a2c5fe:	b89ff0ef          	jal	ra,a2c186 <sfc_port_unlock>
  a2c602:	8522                	mv	a0,s0
  a2c604:	8184                	popret	{ra,s0-s6},48
  a2c606:	0911                	addi	s2,s2,4
  a2c608:	9a4e                	add	s4,s4,s3
  a2c60a:	413484b3          	sub	s1,s1,s3
  a2c60e:	ffc4f993          	andi	s3,s1,-4
  a2c612:	02098163          	beqz	s3,a2c634 <uapi_sfc_reg_read+0xa4>
  a2c616:	00a0 5820 079f      	l.li	a5,0xa05820
  a2c61c:	43d4                	lw	a3,4(a5)
  a2c61e:	864e                	mv	a2,s3
  a2c620:	85d2                	mv	a1,s4
  a2c622:	854a                	mv	a0,s2
  a2c624:	fa0ff0ef          	jal	ra,a2bdc4 <hal_sfc_reg_read>
  a2c628:	842a                	mv	s0,a0
  a2c62a:	f969                	bnez	a0,a2c5fc <uapi_sfc_reg_read+0x6c>
  a2c62c:	9a4e                	add	s4,s4,s3
  a2c62e:	994e                	add	s2,s2,s3
  a2c630:	413484b3          	sub	s1,s1,s3
  a2c634:	4401                	li	s0,0
  a2c636:	d0f9                	beqz	s1,a2c5fc <uapi_sfc_reg_read+0x6c>
  a2c638:	00a0 5820 079f      	l.li	a5,0xa05820
  a2c63e:	43d4                	lw	a3,4(a5)
  a2c640:	4611                	li	a2,4
  a2c642:	006c                	addi	a1,sp,12
  a2c644:	854a                	mv	a0,s2
  a2c646:	c602                	sw	zero,12(sp)
  a2c648:	f7cff0ef          	jal	ra,a2bdc4 <hal_sfc_reg_read>
  a2c64c:	842a                	mv	s0,a0
  a2c64e:	86a6                	mv	a3,s1
  a2c650:	0070                	addi	a2,sp,12
  a2c652:	85a6                	mv	a1,s1
  a2c654:	8552                	mv	a0,s4
  a2c656:	31a040ef          	jal	ra,a30970 <memcpy_s>
  a2c65a:	d14d                	beqz	a0,a2c5fc <uapi_sfc_reg_read+0x6c>
  a2c65c:	80000437          	lui	s0,0x80000
  a2c660:	0411                	addi	s0,s0,4 # 80000004 <_gp_+0x7f5cb848>
  a2c662:	bf69                	j	a2c5fc <uapi_sfc_reg_read+0x6c>

00a2c664 <uapi_sfc_reg_write>:
  a2c664:	8198                	push	{ra,s0-s7},-64
  a2c666:	8a2e                	mv	s4,a1
  a2c668:	85b2                	mv	a1,a2
  a2c66a:	84aa                	mv	s1,a0
  a2c66c:	8932                	mv	s2,a2
  a2c66e:	dfdff0ef          	jal	ra,a2c46a <check_opt_param>
  a2c672:	842a                	mv	s0,a0
  a2c674:	e571                	bnez	a0,a2c740 <uapi_sfc_reg_write+0xdc>
  a2c676:	012485b3          	add	a1,s1,s2
  a2c67a:	8526                	mv	a0,s1
  a2c67c:	c2dff0ef          	jal	ra,a2c2a8 <sfc_port_write_lock>
  a2c680:	0034fa93          	andi	s5,s1,3
  a2c684:	8b2a                	mv	s6,a0
  a2c686:	060a8863          	beqz	s5,a2c6f6 <uapi_sfc_reg_write+0x92>
  a2c68a:	4791                	li	a5,4
  a2c68c:	415787b3          	sub	a5,a5,s5
  a2c690:	415484b3          	sub	s1,s1,s5
  a2c694:	0ff97993          	andi	s3,s2,255
  a2c698:	00f96763          	bltu	s2,a5,a2c6a6 <uapi_sfc_reg_write+0x42>
  a2c69c:	4991                	li	s3,4
  a2c69e:	415989b3          	sub	s3,s3,s5
  a2c6a2:	0ff9f993          	andi	s3,s3,255
  a2c6a6:	00a0 5820 0b9f      	l.li	s7,0xa05820
  a2c6ac:	004ba683          	lw	a3,4(s7)
  a2c6b0:	4611                	li	a2,4
  a2c6b2:	006c                	addi	a1,sp,12
  a2c6b4:	8526                	mv	a0,s1
  a2c6b6:	f0eff0ef          	jal	ra,a2bdc4 <hal_sfc_reg_read>
  a2c6ba:	842a                	mv	s0,a0
  a2c6bc:	ed09                	bnez	a0,a2c6d6 <uapi_sfc_reg_write+0x72>
  a2c6be:	007c                	addi	a5,sp,12
  a2c6c0:	86ce                	mv	a3,s3
  a2c6c2:	8652                	mv	a2,s4
  a2c6c4:	85ce                	mv	a1,s3
  a2c6c6:	01578533          	add	a0,a5,s5
  a2c6ca:	2a6040ef          	jal	ra,a30970 <memcpy_s>
  a2c6ce:	c901                	beqz	a0,a2c6de <uapi_sfc_reg_write+0x7a>
  a2c6d0:	80000437          	lui	s0,0x80000
  a2c6d4:	0411                	addi	s0,s0,4 # 80000004 <_gp_+0x7f5cb848>
  a2c6d6:	855a                	mv	a0,s6
  a2c6d8:	c03ff0ef          	jal	ra,a2c2da <sfc_port_write_unlock>
  a2c6dc:	a095                	j	a2c740 <uapi_sfc_reg_write+0xdc>
  a2c6de:	00cba683          	lw	a3,12(s7)
  a2c6e2:	8526                	mv	a0,s1
  a2c6e4:	4611                	li	a2,4
  a2c6e6:	006c                	addi	a1,sp,12
  a2c6e8:	f68ff0ef          	jal	ra,a2be50 <hal_sfc_reg_write>
  a2c6ec:	842a                	mv	s0,a0
  a2c6ee:	0491                	addi	s1,s1,4
  a2c6f0:	9a4e                	add	s4,s4,s3
  a2c6f2:	41390933          	sub	s2,s2,s3
  a2c6f6:	ffc97993          	andi	s3,s2,-4
  a2c6fa:	fc71                	bnez	s0,a2c6d6 <uapi_sfc_reg_write+0x72>
  a2c6fc:	02098063          	beqz	s3,a2c71c <uapi_sfc_reg_write+0xb8>
  a2c700:	00a0 5820 079f      	l.li	a5,0xa05820
  a2c706:	47d4                	lw	a3,12(a5)
  a2c708:	85d2                	mv	a1,s4
  a2c70a:	8526                	mv	a0,s1
  a2c70c:	864e                	mv	a2,s3
  a2c70e:	f42ff0ef          	jal	ra,a2be50 <hal_sfc_reg_write>
  a2c712:	9a4e                	add	s4,s4,s3
  a2c714:	94ce                	add	s1,s1,s3
  a2c716:	41390933          	sub	s2,s2,s3
  a2c71a:	e131                	bnez	a0,a2c75e <uapi_sfc_reg_write+0xfa>
  a2c71c:	fa090de3          	beqz	s2,a2c6d6 <uapi_sfc_reg_write+0x72>
  a2c720:	00a0 5820 041f      	l.li	s0,0xa05820
  a2c726:	4054                	lw	a3,4(s0)
  a2c728:	4611                	li	a2,4
  a2c72a:	006c                	addi	a1,sp,12
  a2c72c:	8526                	mv	a0,s1
  a2c72e:	e96ff0ef          	jal	ra,a2bdc4 <hal_sfc_reg_read>
  a2c732:	c909                	beqz	a0,a2c744 <uapi_sfc_reg_write+0xe0>
  a2c734:	855a                	mv	a0,s6
  a2c736:	80000437          	lui	s0,0x80000
  a2c73a:	ba1ff0ef          	jal	ra,a2c2da <sfc_port_write_unlock>
  a2c73e:	0411                	addi	s0,s0,4 # 80000004 <_gp_+0x7f5cb848>
  a2c740:	8522                	mv	a0,s0
  a2c742:	8194                	popret	{ra,s0-s7},64
  a2c744:	86ca                	mv	a3,s2
  a2c746:	8652                	mv	a2,s4
  a2c748:	4591                	li	a1,4
  a2c74a:	0068                	addi	a0,sp,12
  a2c74c:	224040ef          	jal	ra,a30970 <memcpy_s>
  a2c750:	f175                	bnez	a0,a2c734 <uapi_sfc_reg_write+0xd0>
  a2c752:	4454                	lw	a3,12(s0)
  a2c754:	4611                	li	a2,4
  a2c756:	006c                	addi	a1,sp,12
  a2c758:	8526                	mv	a0,s1
  a2c75a:	ef6ff0ef          	jal	ra,a2be50 <hal_sfc_reg_write>
  a2c75e:	842a                	mv	s0,a0
  a2c760:	bf9d                	j	a2c6d6 <uapi_sfc_reg_write+0x72>

00a2c762 <uapi_sfc_reg_erase>:
  a2c762:	80a8                	push	{ra,s0-s8},-48
  a2c764:	892a                	mv	s2,a0
  a2c766:	842e                	mv	s0,a1
  a2c768:	d03ff0ef          	jal	ra,a2c46a <check_opt_param>
  a2c76c:	e921                	bnez	a0,a2c7bc <uapi_sfc_reg_erase+0x5a>
  a2c76e:	008905b3          	add	a1,s2,s0
  a2c772:	77fd                	lui	a5,0xfffff
  a2c774:	01459713          	slli	a4,a1,0x14
  a2c778:	00f974b3          	and	s1,s2,a5
  a2c77c:	cf41                	beqz	a4,a2c814 <uapi_sfc_reg_erase+0xb2>
  a2c77e:	8fed                	and	a5,a5,a1
  a2c780:	6705                	lui	a4,0x1
  a2c782:	80000537          	lui	a0,0x80000
  a2c786:	97ba                	add	a5,a5,a4
  a2c788:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2c78a:	02991963          	bne	s2,s1,a2c7bc <uapi_sfc_reg_erase+0x5a>
  a2c78e:	02f59763          	bne	a1,a5,a2c7bc <uapi_sfc_reg_erase+0x5a>
  a2c792:	8526                	mv	a0,s1
  a2c794:	40958a33          	sub	s4,a1,s1
  a2c798:	6b11                	lui	s6,0x4
  a2c79a:	b0fff0ef          	jal	ra,a2c2a8 <sfc_port_write_lock>
  a2c79e:	8c2a                	mv	s8,a0
  a2c7a0:	4901                	li	s2,0
  a2c7a2:	4981                	li	s3,0
  a2c7a4:	4a81                	li	s5,0
  a2c7a6:	00a0 5820 0b9f      	l.li	s7,0xa05820
  a2c7ac:	1b7d                	addi	s6,s6,-1 # 3fff <ccause+0x303d>
  a2c7ae:	040a1d63          	bnez	s4,a2c808 <uapi_sfc_reg_erase+0xa6>
  a2c7b2:	4401                	li	s0,0
  a2c7b4:	8562                	mv	a0,s8
  a2c7b6:	b25ff0ef          	jal	ra,a2c2da <sfc_port_write_unlock>
  a2c7ba:	8522                	mv	a0,s0
  a2c7bc:	80a4                	popret	{ra,s0-s8},48
  a2c7be:	010ba703          	lw	a4,16(s7)
  a2c7c2:	04f7071b          	addshf	a4,a4,a5,sll,2
  a2c7c6:	00072903          	lw	s2,0(a4) # 1000 <ccause+0x3e>
  a2c7ca:	00e95993          	srli	s3,s2,0xe
  a2c7ce:	8ace                	mv	s5,s3
  a2c7d0:	013a6663          	bltu	s4,s3,a2c7dc <uapi_sfc_reg_erase+0x7a>
  a2c7d4:	fff98713          	addi	a4,s3,-1
  a2c7d8:	8f65                	and	a4,a4,s1
  a2c7da:	cb01                	beqz	a4,a2c7ea <uapi_sfc_reg_erase+0x88>
  a2c7dc:	0785                	addi	a5,a5,1 # fffff001 <_gp_+0xff5ca845>
  a2c7de:	008ba703          	lw	a4,8(s7)
  a2c7e2:	fce7eee3          	bltu	a5,a4,a2c7be <uapi_sfc_reg_erase+0x5c>
  a2c7e6:	02e78363          	beq	a5,a4,a2c80c <uapi_sfc_reg_erase+0xaa>
  a2c7ea:	01697933          	and	s2,s2,s6
  a2c7ee:	1d39291b          	orshf	s2,s2,s3,sll,14
  a2c7f2:	4601                	li	a2,0
  a2c7f4:	85ca                	mv	a1,s2
  a2c7f6:	8526                	mv	a0,s1
  a2c7f8:	eb8ff0ef          	jal	ra,a2beb0 <hal_sfc_reg_erase>
  a2c7fc:	842a                	mv	s0,a0
  a2c7fe:	f95d                	bnez	a0,a2c7b4 <uapi_sfc_reg_erase+0x52>
  a2c800:	415a0a33          	sub	s4,s4,s5
  a2c804:	94d6                	add	s1,s1,s5
  a2c806:	b765                	j	a2c7ae <uapi_sfc_reg_erase+0x4c>
  a2c808:	4785                	li	a5,1
  a2c80a:	bfd1                	j	a2c7de <uapi_sfc_reg_erase+0x7c>
  a2c80c:	8000 1347 041f      	l.li	s0,0x80001347
  a2c812:	b74d                	j	a2c7b4 <uapi_sfc_reg_erase+0x52>
  a2c814:	f6990fe3          	beq	s2,s1,a2c792 <uapi_sfc_reg_erase+0x30>
  a2c818:	80000537          	lui	a0,0x80000
  a2c81c:	0505                	addi	a0,a0,1 # 80000001 <_gp_+0x7f5cb845>
  a2c81e:	bf79                	j	a2c7bc <uapi_sfc_reg_erase+0x5a>

00a2c820 <efuse_read_item.part.1>:
  a2c820:	8188                	push	{ra,s0-s6},-48
  a2c822:	00a3 27fc 079f      	l.li	a5,0xa327fc
  a2c828:	06a7955b          	muliadd	a0,a5,a0,6
  a2c82c:	2126                	lhu	s1,2(a0)
  a2c82e:	00748a13          	addi	s4,s1,7
  a2c832:	003a5a13          	srli	s4,s4,0x3
  a2c836:	07466f63          	bltu	a2,s4,a2c8b4 <efuse_read_item.part.1+0x94>
  a2c83a:	00055903          	lhu	s2,0(a0)
  a2c83e:	86b2                	mv	a3,a2
  a2c840:	89ae                	mv	s3,a1
  a2c842:	4601                	li	a2,0
  a2c844:	85b6                	mv	a1,a3
  a2c846:	854e                	mv	a0,s3
  a2c848:	1a4040ef          	jal	ra,a309ec <memset_s>
  a2c84c:	009967b3          	or	a5,s2,s1
  a2c850:	8b9d                	andi	a5,a5,7
  a2c852:	844a                	mv	s0,s2
  a2c854:	cb91                	beqz	a5,a2c868 <efuse_read_item.part.1+0x48>
  a2c856:	6a41                	lui	s4,0x10
  a2c858:	94ca                	add	s1,s1,s2
  a2c85a:	4aa1                	li	s5,8
  a2c85c:	1a7d                	addi	s4,s4,-1 # ffff <g_intheap_size+0x3>
  a2c85e:	4b05                	li	s6,1
  a2c860:	00944b63          	blt	s0,s1,a2c876 <efuse_read_item.part.1+0x56>
  a2c864:	4501                	li	a0,0
  a2c866:	a039                	j	a2c874 <efuse_read_item.part.1+0x54>
  a2c868:	8652                	mv	a2,s4
  a2c86a:	00395593          	srli	a1,s2,0x3
  a2c86e:	854e                	mv	a0,s3
  a2c870:	ff9fd0ef          	jal	ra,a2a868 <uapi_efuse_read_buffer>
  a2c874:	8184                	popret	{ra,s0-s6},48
  a2c876:	00747613          	andi	a2,s0,7
  a2c87a:	00345593          	srli	a1,s0,0x3
  a2c87e:	00f10513          	addi	a0,sp,15
  a2c882:	f91fd0ef          	jal	ra,a2a812 <uapi_efuse_read_bit>
  a2c886:	f57d                	bnez	a0,a2c874 <efuse_read_item.part.1+0x54>
  a2c888:	00f14783          	lbu	a5,15(sp)
  a2c88c:	c38d                	beqz	a5,a2c8ae <efuse_read_item.part.1+0x8e>
  a2c88e:	412407b3          	sub	a5,s0,s2
  a2c892:	0357c733          	div	a4,a5,s5
  a2c896:	0357e7b3          	rem	a5,a5,s5
  a2c89a:	01477733          	and	a4,a4,s4
  a2c89e:	974e                	add	a4,a4,s3
  a2c8a0:	2314                	lbu	a3,0(a4)
  a2c8a2:	0147f7b3          	and	a5,a5,s4
  a2c8a6:	00fb17b3          	sll	a5,s6,a5
  a2c8aa:	8fd5                	or	a5,a5,a3
  a2c8ac:	a31c                	sb	a5,0(a4)
  a2c8ae:	0405                	addi	s0,s0,1
  a2c8b0:	9c21                	uxth	s0
  a2c8b2:	b77d                	j	a2c860 <efuse_read_item.part.1+0x40>
  a2c8b4:	557d                	li	a0,-1
  a2c8b6:	bf7d                	j	a2c874 <efuse_read_item.part.1+0x54>

00a2c8b8 <efuse_port_register_hal_funcs>:
  a2c8b8:	8018                	push	{ra},-16
  a2c8ba:	9fafe0ef          	jal	ra,a2aab4 <hal_efuse_funcs_get>
  a2c8be:	8010                	pop	{ra},16
  a2c8c0:	802fe06f          	j	a2a8c2 <hal_efuse_register_funcs>

00a2c8c4 <hal_efuse_get_region>:
  a2c8c4:	8121                	srli	a0,a0,0x8
  a2c8c6:	9d01                	uxtb	a0
  a2c8c8:	8082                	ret

00a2c8ca <hal_efuse_get_byte_offset>:
  a2c8ca:	9d01                	uxtb	a0
  a2c8cc:	8082                	ret

00a2c8ce <efuse_read_item>:
  a2c8ce:	c589                	beqz	a1,a2c8d8 <efuse_read_item+0xa>
  a2c8d0:	1205723b          	bgeui	a0,18,a2c8d8 <efuse_read_item+0xa>
  a2c8d4:	f4dff06f          	j	a2c820 <efuse_read_item.part.1>
  a2c8d8:	557d                	li	a0,-1
  a2c8da:	8082                	ret

00a2c8dc <set_efuse_period>:
  a2c8dc:	8018                	push	{ra},-16
  a2c8de:	2895                	jal	ra,a2c952 <get_tcxo_freq>
  a2c8e0:	010513bb          	bnei	a0,1,a2c8ee <set_efuse_period+0x12>
  a2c8e4:	02900513          	li	a0,41
  a2c8e8:	8010                	pop	{ra},16
  a2c8ea:	9c0fe06f          	j	a2aaaa <hal_efuse_set_clock_period>
  a2c8ee:	4565                	li	a0,25
  a2c8f0:	bfe5                	j	a2c8e8 <set_efuse_period+0xc>

00a2c8f2 <malloc_port>:
  a2c8f2:	fabfc06f          	j	a2989c <malloc>

00a2c8f6 <malloc_port_init>:
  a2c8f6:	8118                	push	{ra},-32
  a2c8f8:	00a2 97c2 079f      	l.li	a5,0xa297c2
  a2c8fe:	c23e                	sw	a5,4(sp)
  a2c900:	00a2 98d2 079f      	l.li	a5,0xa298d2
  a2c906:	c63e                	sw	a5,12(sp)
  a2c908:	0048                	addi	a0,sp,4
  a2c90a:	00a2 c8f2 079f      	l.li	a5,0xa2c8f2
  a2c910:	c43e                	sw	a5,8(sp)
  a2c912:	e5dfc0ef          	jal	ra,a2976e <malloc_register_funcs>
  a2c916:	00a06537          	lui	a0,0xa06
  a2c91a:	0000 fffc 079f      	l.li	a5,0xfffc
  a2c920:	b0850593          	addi	a1,a0,-1272 # a05b08 <__heap_start>
  a2c924:	95be                	add	a1,a1,a5
  a2c926:	b0850513          	addi	a0,a0,-1272
  a2c92a:	e99fc0ef          	jal	ra,a297c2 <malloc_init>
  a2c92e:	8114                	popret	{ra},32

00a2c930 <reboot_port_reboot_chip>:
  a2c930:	8018                	push	{ra},-16
  a2c932:	e6bfd0ef          	jal	ra,a2a79c <uapi_watchdog_deinit>
  a2c936:	0000 0bb8 051f      	l.li	a0,0xbb8
  a2c93c:	959fd0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2c940:	40002737          	lui	a4,0x40002
  a2c944:	11072783          	lw	a5,272(a4) # 40002110 <_gp_+0x3f5cd954>
  a2c948:	0047e793          	ori	a5,a5,4
  a2c94c:	10f72823          	sw	a5,272(a4)
  a2c950:	8014                	popret	{ra},16

00a2c952 <get_tcxo_freq>:
  a2c952:	400007b7          	lui	a5,0x40000
  a2c956:	4bc8                	lw	a0,20(a5)
  a2c958:	8905                	andi	a0,a0,1
  a2c95a:	8082                	ret

00a2c95c <boot_clock_adapt>:
  a2c95c:	8028                	push	{ra,s0},-16
  a2c95e:	ff5ff0ef          	jal	ra,a2c952 <get_tcxo_freq>
  a2c962:	010518bb          	bnei	a0,1,a2c984 <boot_clock_adapt+0x28>
  a2c966:	016e3437          	lui	s0,0x16e3
  a2c96a:	60040593          	addi	a1,s0,1536 # 16e3600 <_gp_+0xcaee44>
  a2c96e:	4501                	li	a0,0
  a2c970:	eacff0ef          	jal	ra,a2c01c <uart_port_set_clock_value>
  a2c974:	4561                	li	a0,24
  a2c976:	f64ff0ef          	jal	ra,a2c0da <tcxo_porting_ticks_per_usec_set>
  a2c97a:	60040513          	addi	a0,s0,1536
  a2c97e:	8020                	pop	{ra,s0},16
  a2c980:	f76ff06f          	j	a2c0f6 <watchdog_port_set_clock>
  a2c984:	02626437          	lui	s0,0x2626
  a2c988:	a0040593          	addi	a1,s0,-1536 # 2625a00 <_gp_+0x1bf1244>
  a2c98c:	4501                	li	a0,0
  a2c98e:	e8eff0ef          	jal	ra,a2c01c <uart_port_set_clock_value>
  a2c992:	02800513          	li	a0,40
  a2c996:	f44ff0ef          	jal	ra,a2c0da <tcxo_porting_ticks_per_usec_set>
  a2c99a:	a0040513          	addi	a0,s0,-1536
  a2c99e:	b7c5                	j	a2c97e <boot_clock_adapt+0x22>

00a2c9a0 <switch_flash_clock_to_pll>:
  a2c9a0:	8028                	push	{ra,s0},-16
  a2c9a2:	40003437          	lui	s0,0x40003
  a2c9a6:	4785                	li	a5,1
  a2c9a8:	4af42223          	sw	a5,1188(s0) # 400034a4 <_gp_+0x3f5cece8>
  a2c9ac:	4505                	li	a0,1
  a2c9ae:	8e7fd0ef          	jal	ra,a2a294 <uapi_tcxo_delay_us>
  a2c9b2:	478d                	li	a5,3
  a2c9b4:	4af42223          	sw	a5,1188(s0)
  a2c9b8:	44001737          	lui	a4,0x44001
  a2c9bc:	13472783          	lw	a5,308(a4) # 44001134 <_gp_+0x435cc978>
  a2c9c0:	000406b7          	lui	a3,0x40
  a2c9c4:	8fd5                	or	a5,a5,a3
  a2c9c6:	12f72a23          	sw	a5,308(a4)
  a2c9ca:	8024                	popret	{ra,s0},16

00a2c9cc <config_sfc_ctrl_ds>:
  a2c9cc:	4400e7b7          	lui	a5,0x4400e
  a2c9d0:	8687a703          	lw	a4,-1944(a5) # 4400d868 <_gp_+0x435d90ac>
  a2c9d4:	f8f77713          	andi	a4,a4,-113
  a2c9d8:	03076713          	ori	a4,a4,48
  a2c9dc:	86e7a423          	sw	a4,-1944(a5)
  a2c9e0:	86c7a703          	lw	a4,-1940(a5)
  a2c9e4:	f8f77713          	andi	a4,a4,-113
  a2c9e8:	02076713          	ori	a4,a4,32
  a2c9ec:	86e7a623          	sw	a4,-1940(a5)
  a2c9f0:	8707a703          	lw	a4,-1936(a5)
  a2c9f4:	f8f77713          	andi	a4,a4,-113
  a2c9f8:	02076713          	ori	a4,a4,32
  a2c9fc:	86e7a823          	sw	a4,-1936(a5)
  a2ca00:	8747a703          	lw	a4,-1932(a5)
  a2ca04:	f8f77713          	andi	a4,a4,-113
  a2ca08:	02076713          	ori	a4,a4,32
  a2ca0c:	86e7aa23          	sw	a4,-1932(a5)
  a2ca10:	8787a703          	lw	a4,-1928(a5)
  a2ca14:	f8f77713          	andi	a4,a4,-113
  a2ca18:	02076713          	ori	a4,a4,32
  a2ca1c:	86e7ac23          	sw	a4,-1928(a5)
  a2ca20:	87c7a703          	lw	a4,-1924(a5)
  a2ca24:	f8f77713          	andi	a4,a4,-113
  a2ca28:	02076713          	ori	a4,a4,32
  a2ca2c:	86e7ae23          	sw	a4,-1924(a5)
  a2ca30:	8082                	ret

00a2ca32 <serial_putc>:
  a2ca32:	8118                	push	{ra},-32
  a2ca34:	00a0 583b 079f      	l.li	a5,0xa0583b
  a2ca3a:	239c                	lbu	a5,0(a5)
  a2ca3c:	00a107a3          	sb	a0,15(sp)
  a2ca40:	eb81                	bnez	a5,a2ca50 <serial_putc+0x1e>
  a2ca42:	4689                	li	a3,2
  a2ca44:	4605                	li	a2,1
  a2ca46:	00f10593          	addi	a1,sp,15
  a2ca4a:	4501                	li	a0,0
  a2ca4c:	c13fd0ef          	jal	ra,a2a65e <uapi_uart_write>
  a2ca50:	8114                	popret	{ra},32

00a2ca52 <serial_puts>:
  a2ca52:	00a0 583b 079f      	l.li	a5,0xa0583b
  a2ca58:	239c                	lbu	a5,0(a5)
  a2ca5a:	ef89                	bnez	a5,a2ca74 <serial_puts+0x22>
  a2ca5c:	8118                	push	{ra},-32
  a2ca5e:	c62a                	sw	a0,12(sp)
  a2ca60:	aacfc0ef          	jal	ra,a28d0c <strlen>
  a2ca64:	45b2                	lw	a1,12(sp)
  a2ca66:	862a                	mv	a2,a0
  a2ca68:	7d000693          	li	a3,2000
  a2ca6c:	4501                	li	a0,0
  a2ca6e:	8110                	pop	{ra},32
  a2ca70:	beffd06f          	j	a2a65e <uapi_uart_write>
  a2ca74:	8082                	ret

00a2ca76 <serial_puthex>:
  a2ca76:	00a0 583b 079f      	l.li	a5,0xa0583b
  a2ca7c:	239c                	lbu	a5,0(a5)
  a2ca7e:	efa9                	bnez	a5,a2cad8 <serial_puthex+0x62>
  a2ca80:	8098                	push	{ra,s0-s7},-48
  a2ca82:	84aa                	mv	s1,a0
  a2ca84:	00a3 2868 051f      	l.li	a0,0xa32868
  a2ca8a:	8a2e                	mv	s4,a1
  a2ca8c:	4401                	li	s0,0
  a2ca8e:	fc5ff0ef          	jal	ra,a2ca52 <serial_puts>
  a2ca92:	4901                	li	s2,0
  a2ca94:	4aa5                	li	s5,9
  a2ca96:	4b1d                	li	s6,7
  a2ca98:	03000b93          	li	s7,48
  a2ca9c:	49a1                	li	s3,8
  a2ca9e:	01c4d713          	srli	a4,s1,0x1c
  a2caa2:	0ff77793          	andi	a5,a4,255
  a2caa6:	03078513          	addi	a0,a5,48
  a2caaa:	00eaf463          	bgeu	s5,a4,a2cab2 <serial_puthex+0x3c>
  a2caae:	03778513          	addi	a0,a5,55
  a2cab2:	0492                	slli	s1,s1,0x4
  a2cab4:	000a0563          	beqz	s4,a2cabe <serial_puthex+0x48>
  a2cab8:	f7bff0ef          	jal	ra,a2ca32 <serial_putc>
  a2cabc:	a811                	j	a2cad0 <serial_puthex+0x5a>
  a2cabe:	fe091de3          	bnez	s2,a2cab8 <serial_puthex+0x42>
  a2cac2:	ff640be3          	beq	s0,s6,a2cab8 <serial_puthex+0x42>
  a2cac6:	01750563          	beq	a0,s7,a2cad0 <serial_puthex+0x5a>
  a2caca:	f69ff0ef          	jal	ra,a2ca32 <serial_putc>
  a2cace:	4905                	li	s2,1
  a2cad0:	0405                	addi	s0,s0,1
  a2cad2:	fd3416e3          	bne	s0,s3,a2ca9e <serial_puthex+0x28>
  a2cad6:	8094                	popret	{ra,s0-s7},48
  a2cad8:	8082                	ret

00a2cada <boot_msg0>:
  a2cada:	8018                	push	{ra},-16
  a2cadc:	f77ff0ef          	jal	ra,a2ca52 <serial_puts>
  a2cae0:	00a3 31f8 051f      	l.li	a0,0xa331f8
  a2cae6:	8010                	pop	{ra},16
  a2cae8:	f6bff06f          	j	a2ca52 <serial_puts>

00a2caec <boot_msg1>:
  a2caec:	8028                	push	{ra,s0},-16
  a2caee:	842e                	mv	s0,a1
  a2caf0:	f63ff0ef          	jal	ra,a2ca52 <serial_puts>
  a2caf4:	8522                	mv	a0,s0
  a2caf6:	4581                	li	a1,0
  a2caf8:	f7fff0ef          	jal	ra,a2ca76 <serial_puthex>
  a2cafc:	00a3 31f8 051f      	l.li	a0,0xa331f8
  a2cb02:	8020                	pop	{ra,s0},16
  a2cb04:	f4fff06f          	j	a2ca52 <serial_puts>

00a2cb08 <boot_msg2>:
  a2cb08:	8128                	push	{ra,s0},-32
  a2cb0a:	842e                	mv	s0,a1
  a2cb0c:	c632                	sw	a2,12(sp)
  a2cb0e:	f45ff0ef          	jal	ra,a2ca52 <serial_puts>
  a2cb12:	4581                	li	a1,0
  a2cb14:	8522                	mv	a0,s0
  a2cb16:	f61ff0ef          	jal	ra,a2ca76 <serial_puthex>
  a2cb1a:	00a3 1f84 051f      	l.li	a0,0xa31f84
  a2cb20:	f33ff0ef          	jal	ra,a2ca52 <serial_puts>
  a2cb24:	4632                	lw	a2,12(sp)
  a2cb26:	4581                	li	a1,0
  a2cb28:	8532                	mv	a0,a2
  a2cb2a:	f4dff0ef          	jal	ra,a2ca76 <serial_puthex>
  a2cb2e:	00a3 31f8 051f      	l.li	a0,0xa331f8
  a2cb34:	8120                	pop	{ra,s0},32
  a2cb36:	f1dff06f          	j	a2ca52 <serial_puts>

00a2cb3a <hiburn_uart_init>:
  a2cb3a:	8228                	push	{ra,s0},-48
  a2cb3c:	00a3 3f44 079f      	l.li	a5,0xa33f44
  a2cb42:	00078023          	sb	zero,0(a5)
  a2cb46:	02900793          	li	a5,41
  a2cb4a:	00f10623          	sb	a5,12(sp)
  a2cb4e:	07300793          	li	a5,115
  a2cb52:	00f107a3          	sb	a5,15(sp)
  a2cb56:	00f10723          	sb	a5,14(sp)
  a2cb5a:	411c                	lw	a5,0(a0)
  a2cb5c:	02a00713          	li	a4,42
  a2cb60:	00e106a3          	sb	a4,13(sp)
  a2cb64:	c83e                	sw	a5,16(sp)
  a2cb66:	215c                	lbu	a5,4(a0)
  a2cb68:	2178                	lbu	a4,6(a0)
  a2cb6a:	00010423          	sb	zero,8(sp)
  a2cb6e:	17ed                	addi	a5,a5,-5
  a2cb70:	00f10a23          	sb	a5,20(sp)
  a2cb74:	fff70793          	addi	a5,a4,-1
  a2cb78:	9f81                	uxtb	a5
  a2cb7a:	0207fdbb          	bgeui	a5,2,a2cbb0 <hiburn_uart_init+0x76>
  a2cb7e:	00e10b23          	sb	a4,22(sp)
  a2cb82:	315c                	lbu	a5,5(a0)
  a2cb84:	4405                	li	s0,1
  a2cb86:	0838                	addi	a4,sp,24
  a2cb88:	17fd                	addi	a5,a5,-1
  a2cb8a:	00f10aa3          	sb	a5,21(sp)
  a2cb8e:	4681                	li	a3,0
  a2cb90:	003c                	addi	a5,sp,8
  a2cb92:	0810                	addi	a2,sp,16
  a2cb94:	006c                	addi	a1,sp,12
  a2cb96:	4501                	li	a0,0
  a2cb98:	cc3e                	sw	a5,24(sp)
  a2cb9a:	ce22                	sw	s0,28(sp)
  a2cb9c:	a05fd0ef          	jal	ra,a2a5a0 <uapi_uart_init>
  a2cba0:	4501                	li	a0,0
  a2cba2:	ca0ff0ef          	jal	ra,a2c042 <uart_port_unregister_irq>
  a2cba6:	00a0 583a 079f      	l.li	a5,0xa0583a
  a2cbac:	a380                	sb	s0,0(a5)
  a2cbae:	8224                	popret	{ra,s0},48
  a2cbb0:	00010b23          	sb	zero,22(sp)
  a2cbb4:	b7f9                	j	a2cb82 <hiburn_uart_init+0x48>

00a2cbb6 <set_reset_count>:
  a2cbb6:	40000737          	lui	a4,0x40000
  a2cbba:	471c                	lw	a5,8(a4)
  a2cbbc:	893d                	andi	a0,a0,15
  a2cbbe:	f0f7f793          	andi	a5,a5,-241
  a2cbc2:	08a7a79b          	orshf	a5,a5,a0,sll,4
  a2cbc6:	c71c                	sw	a5,8(a4)
  a2cbc8:	8082                	ret

00a2cbca <update_reset_count>:
  a2cbca:	8038                	push	{ra,s0-s1},-16
  a2cbcc:	400007b7          	lui	a5,0x40000
  a2cbd0:	4780                	lw	s0,8(a5)
  a2cbd2:	00a3 286c 051f      	l.li	a0,0xa3286c
  a2cbd8:	8011                	srli	s0,s0,0x4
  a2cbda:	883d                	andi	s0,s0,15
  a2cbdc:	00140493          	addi	s1,s0,1
  a2cbe0:	85a6                	mv	a1,s1
  a2cbe2:	f0bff0ef          	jal	ra,a2caec <boot_msg1>
  a2cbe6:	06400513          	li	a0,100
  a2cbea:	e5afd0ef          	jal	ra,a2a244 <uapi_tcxo_delay_ms>
  a2cbee:	0f04033b          	beqi	s0,15,a2cbfa <update_reset_count+0x30>
  a2cbf2:	8526                	mv	a0,s1
  a2cbf4:	8030                	pop	{ra,s0-s1},16
  a2cbf6:	fc1ff06f          	j	a2cbb6 <set_reset_count>
  a2cbfa:	b6dfd0ef          	jal	ra,a2a766 <uapi_watchdog_disable>
  a2cbfe:	10500073          	wfi
  a2cc02:	a001                	j	a2cc02 <update_reset_count+0x38>

00a2cc04 <interrupt0_handler>:
  a2cc04:	8028                	push	{ra,s0},-16
  a2cc06:	040007b7          	lui	a5,0x4000
  a2cc0a:	3047b7f3          	csrrc	a5,mie,a5
  a2cc0e:	00a06437          	lui	s0,0xa06
  a2cc12:	83c40713          	addi	a4,s0,-1988 # a0583c <g_interrupt_running>
  a2cc16:	431c                	lw	a5,0(a4)
  a2cc18:	83c40413          	addi	s0,s0,-1988
  a2cc1c:	0785                	addi	a5,a5,1 # 4000001 <_gp_+0x35cb845>
  a2cc1e:	c31c                	sw	a5,0(a4)
  a2cc20:	22c9                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cc22:	5538                	lw	a4,104(a0)
  a2cc24:	00a2 d204 079f      	l.li	a5,0xa2d204
  a2cc2a:	00f70563          	beq	a4,a5,a2cc34 <interrupt0_handler+0x30>
  a2cc2e:	2a55                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cc30:	553c                	lw	a5,104(a0)
  a2cc32:	9782                	jalr	a5
  a2cc34:	040007b7          	lui	a5,0x4000
  a2cc38:	3047a7f3          	csrrs	a5,mie,a5
  a2cc3c:	401c                	lw	a5,0(s0)
  a2cc3e:	17fd                	addi	a5,a5,-1 # 3ffffff <_gp_+0x35cb843>
  a2cc40:	c01c                	sw	a5,0(s0)
  a2cc42:	8024                	popret	{ra,s0},16

00a2cc44 <interrupt1_handler>:
  a2cc44:	8028                	push	{ra,s0},-16
  a2cc46:	080007b7          	lui	a5,0x8000
  a2cc4a:	3047b7f3          	csrrc	a5,mie,a5
  a2cc4e:	00a06437          	lui	s0,0xa06
  a2cc52:	83c40713          	addi	a4,s0,-1988 # a0583c <g_interrupt_running>
  a2cc56:	431c                	lw	a5,0(a4)
  a2cc58:	83c40413          	addi	s0,s0,-1988
  a2cc5c:	0785                	addi	a5,a5,1 # 8000001 <_gp_+0x75cb845>
  a2cc5e:	c31c                	sw	a5,0(a4)
  a2cc60:	2249                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cc62:	5578                	lw	a4,108(a0)
  a2cc64:	00a2 d204 079f      	l.li	a5,0xa2d204
  a2cc6a:	00f70563          	beq	a4,a5,a2cc74 <interrupt1_handler+0x30>
  a2cc6e:	2a95                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cc70:	557c                	lw	a5,108(a0)
  a2cc72:	9782                	jalr	a5
  a2cc74:	080007b7          	lui	a5,0x8000
  a2cc78:	3047a7f3          	csrrs	a5,mie,a5
  a2cc7c:	401c                	lw	a5,0(s0)
  a2cc7e:	17fd                	addi	a5,a5,-1 # 7ffffff <_gp_+0x75cb843>
  a2cc80:	c01c                	sw	a5,0(s0)
  a2cc82:	8024                	popret	{ra,s0},16

00a2cc84 <interrupt2_handler>:
  a2cc84:	8028                	push	{ra,s0},-16
  a2cc86:	100007b7          	lui	a5,0x10000
  a2cc8a:	3047b7f3          	csrrc	a5,mie,a5
  a2cc8e:	00a06437          	lui	s0,0xa06
  a2cc92:	83c40713          	addi	a4,s0,-1988 # a0583c <g_interrupt_running>
  a2cc96:	431c                	lw	a5,0(a4)
  a2cc98:	83c40413          	addi	s0,s0,-1988
  a2cc9c:	0785                	addi	a5,a5,1 # 10000001 <_gp_+0xf5cb845>
  a2cc9e:	c31c                	sw	a5,0(a4)
  a2cca0:	2289                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cca2:	5938                	lw	a4,112(a0)
  a2cca4:	00a2 d204 079f      	l.li	a5,0xa2d204
  a2ccaa:	00f70563          	beq	a4,a5,a2ccb4 <interrupt2_handler+0x30>
  a2ccae:	2a15                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2ccb0:	593c                	lw	a5,112(a0)
  a2ccb2:	9782                	jalr	a5
  a2ccb4:	100007b7          	lui	a5,0x10000
  a2ccb8:	3047a7f3          	csrrs	a5,mie,a5
  a2ccbc:	401c                	lw	a5,0(s0)
  a2ccbe:	17fd                	addi	a5,a5,-1 # fffffff <_gp_+0xf5cb843>
  a2ccc0:	c01c                	sw	a5,0(s0)
  a2ccc2:	8024                	popret	{ra,s0},16

00a2ccc4 <interrupt3_handler>:
  a2ccc4:	8028                	push	{ra,s0},-16
  a2ccc6:	200007b7          	lui	a5,0x20000
  a2ccca:	3047b7f3          	csrrc	a5,mie,a5
  a2ccce:	00a06437          	lui	s0,0xa06
  a2ccd2:	83c40713          	addi	a4,s0,-1988 # a0583c <g_interrupt_running>
  a2ccd6:	431c                	lw	a5,0(a4)
  a2ccd8:	83c40413          	addi	s0,s0,-1988
  a2ccdc:	0785                	addi	a5,a5,1 # 20000001 <_gp_+0x1f5cb845>
  a2ccde:	c31c                	sw	a5,0(a4)
  a2cce0:	2209                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cce2:	5978                	lw	a4,116(a0)
  a2cce4:	00a2 d204 079f      	l.li	a5,0xa2d204
  a2ccea:	00f70563          	beq	a4,a5,a2ccf4 <interrupt3_handler+0x30>
  a2ccee:	28d5                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2ccf0:	597c                	lw	a5,116(a0)
  a2ccf2:	9782                	jalr	a5
  a2ccf4:	200007b7          	lui	a5,0x20000
  a2ccf8:	3047a7f3          	csrrs	a5,mie,a5
  a2ccfc:	401c                	lw	a5,0(s0)
  a2ccfe:	17fd                	addi	a5,a5,-1 # 1fffffff <_gp_+0x1f5cb843>
  a2cd00:	c01c                	sw	a5,0(s0)
  a2cd02:	8024                	popret	{ra,s0},16

00a2cd04 <interrupt4_handler>:
  a2cd04:	8028                	push	{ra,s0},-16
  a2cd06:	400007b7          	lui	a5,0x40000
  a2cd0a:	3047b7f3          	csrrc	a5,mie,a5
  a2cd0e:	00a06437          	lui	s0,0xa06
  a2cd12:	83c40713          	addi	a4,s0,-1988 # a0583c <g_interrupt_running>
  a2cd16:	431c                	lw	a5,0(a4)
  a2cd18:	83c40413          	addi	s0,s0,-1988
  a2cd1c:	0785                	addi	a5,a5,1 # 40000001 <_gp_+0x3f5cb845>
  a2cd1e:	c31c                	sw	a5,0(a4)
  a2cd20:	20c9                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cd22:	5d38                	lw	a4,120(a0)
  a2cd24:	00a2 d204 079f      	l.li	a5,0xa2d204
  a2cd2a:	00f70563          	beq	a4,a5,a2cd34 <interrupt4_handler+0x30>
  a2cd2e:	2855                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cd30:	5d3c                	lw	a5,120(a0)
  a2cd32:	9782                	jalr	a5
  a2cd34:	400007b7          	lui	a5,0x40000
  a2cd38:	3047a7f3          	csrrs	a5,mie,a5
  a2cd3c:	401c                	lw	a5,0(s0)
  a2cd3e:	17fd                	addi	a5,a5,-1 # 3fffffff <_gp_+0x3f5cb843>
  a2cd40:	c01c                	sw	a5,0(s0)
  a2cd42:	8024                	popret	{ra,s0},16

00a2cd44 <interrupt5_handler>:
  a2cd44:	8028                	push	{ra,s0},-16
  a2cd46:	800007b7          	lui	a5,0x80000
  a2cd4a:	3047b7f3          	csrrc	a5,mie,a5
  a2cd4e:	00a06437          	lui	s0,0xa06
  a2cd52:	83c40713          	addi	a4,s0,-1988 # a0583c <g_interrupt_running>
  a2cd56:	431c                	lw	a5,0(a4)
  a2cd58:	83c40413          	addi	s0,s0,-1988
  a2cd5c:	0785                	addi	a5,a5,1 # 80000001 <_gp_+0x7f5cb845>
  a2cd5e:	c31c                	sw	a5,0(a4)
  a2cd60:	2049                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cd62:	5d78                	lw	a4,124(a0)
  a2cd64:	00a2 d204 079f      	l.li	a5,0xa2d204
  a2cd6a:	00f70563          	beq	a4,a5,a2cd74 <interrupt5_handler+0x30>
  a2cd6e:	2895                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cd70:	5d7c                	lw	a5,124(a0)
  a2cd72:	9782                	jalr	a5
  a2cd74:	800007b7          	lui	a5,0x80000
  a2cd78:	3047a7f3          	csrrs	a5,mie,a5
  a2cd7c:	401c                	lw	a5,0(s0)
  a2cd7e:	17fd                	addi	a5,a5,-1 # 7fffffff <_gp_+0x7f5cb843>
  a2cd80:	c01c                	sw	a5,0(s0)
  a2cd82:	8024                	popret	{ra,s0},16

00a2cd84 <local_interrupt_handler>:
  a2cd84:	8038                	push	{ra,s0-s1},-16
  a2cd86:	673040ef          	jal	ra,a31bf8 <global_interrupt_lock>
  a2cd8a:	00a064b7          	lui	s1,0xa06
  a2cd8e:	83c48713          	addi	a4,s1,-1988 # a0583c <g_interrupt_running>
  a2cd92:	431c                	lw	a5,0(a4)
  a2cd94:	0785                	addi	a5,a5,1
  a2cd96:	c31c                	sw	a5,0(a4)
  a2cd98:	679040ef          	jal	ra,a31c10 <global_interrupt_restore>
  a2cd9c:	34202473          	csrr	s0,mcause
  a2cda0:	2089                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cda2:	6785                	lui	a5,0x1
  a2cda4:	17fd                	addi	a5,a5,-1 # fff <ccause+0x3d>
  a2cda6:	8c7d                	and	s0,s0,a5
  a2cda8:	040a                	slli	s0,s0,0x2
  a2cdaa:	9522                	add	a0,a0,s0
  a2cdac:	4118                	lw	a4,0(a0)
  a2cdae:	00a2 cde0 079f      	l.li	a5,0xa2cde0
  a2cdb4:	83c48493          	addi	s1,s1,-1988
  a2cdb8:	00f70663          	beq	a4,a5,a2cdc4 <local_interrupt_handler+0x40>
  a2cdbc:	201d                	jal	ra,a2cde2 <isr_get_ramexceptiontable_addr>
  a2cdbe:	9522                	add	a0,a0,s0
  a2cdc0:	411c                	lw	a5,0(a0)
  a2cdc2:	9782                	jalr	a5
  a2cdc4:	635040ef          	jal	ra,a31bf8 <global_interrupt_lock>
  a2cdc8:	409c                	lw	a5,0(s1)
  a2cdca:	17fd                	addi	a5,a5,-1
  a2cdcc:	c09c                	sw	a5,0(s1)
  a2cdce:	8030                	pop	{ra,s0-s1},16
  a2cdd0:	6410406f          	j	a31c10 <global_interrupt_restore>

00a2cdd4 <interrupt_number_get>:
  a2cdd4:	34202573          	csrr	a0,mcause
  a2cdd8:	6785                	lui	a5,0x1
  a2cdda:	17fd                	addi	a5,a5,-1 # fff <ccause+0x3d>
  a2cddc:	8d7d                	and	a0,a0,a5
  a2cdde:	8082                	ret

00a2cde0 <b_sub_monitor_handler>:
  a2cde0:	a001                	j	a2cde0 <b_sub_monitor_handler>

00a2cde2 <isr_get_ramexceptiontable_addr>:
  a2cde2:	00a2 8420 051f      	l.li	a0,0xa28420
  a2cde8:	8082                	ret

00a2cdea <do_hard_fault_handler>:
  a2cdea:	c13d                	beqz	a0,a2ce50 <do_hard_fault_handler+0x66>
  a2cdec:	8048                	push	{ra,s0-s2},-16
  a2cdee:	00452903          	lw	s2,4(a0)
  a2cdf2:	842a                	mv	s0,a0
  a2cdf4:	00a3 28c4 051f      	l.li	a0,0xa328c4
  a2cdfa:	85ca                	mv	a1,s2
  a2cdfc:	a48ff0ef          	jal	ra,a2c044 <print_str>
  a2ce00:	800007b7          	lui	a5,0x80000
  a2ce04:	07b1                	addi	a5,a5,12 # 8000000c <_gp_+0x7f5cb850>
  a2ce06:	04f91263          	bne	s2,a5,a2ce4a <do_hard_fault_handler+0x60>
  a2ce0a:	00a3 28d8 051f      	l.li	a0,0xa328d8
  a2ce10:	6489                	lui	s1,0x2
  a2ce12:	a32ff0ef          	jal	ra,a2c044 <print_str>
  a2ce16:	04a1                	addi	s1,s1,8 # 2008 <ccause+0x1046>
  a2ce18:	8526                	mv	a0,s1
  a2ce1a:	363000ef          	jal	ra,a2d97c <cpu_utils_set_system_status_by_cause>
  a2ce1e:	4c5c                	lw	a5,28(s0)
  a2ce20:	8522                	mv	a0,s0
  a2ce22:	07c1                	addi	a5,a5,16
  a2ce24:	cc5c                	sw	a5,28(s0)
  a2ce26:	4c1000ef          	jal	ra,a2dae6 <set_exception_info_riscv>
  a2ce2a:	00a0 5854 079f      	l.li	a5,0xa05854
  a2ce30:	439c                	lw	a5,0(a5)
  a2ce32:	c781                	beqz	a5,a2ce3a <do_hard_fault_handler+0x50>
  a2ce34:	85a2                	mv	a1,s0
  a2ce36:	854a                	mv	a0,s2
  a2ce38:	9782                	jalr	a5
  a2ce3a:	85a6                	mv	a1,s1
  a2ce3c:	4509                	li	a0,2
  a2ce3e:	347000ef          	jal	ra,a2d984 <cpu_utils_reset_chip_with_log>
  a2ce42:	4501                	li	a0,0
  a2ce44:	8040                	pop	{ra,s0-s2},16
  a2ce46:	8ddfd06f          	j	a2a722 <uapi_watchdog_enable>
  a2ce4a:	6489                	lui	s1,0x2
  a2ce4c:	0491                	addi	s1,s1,4 # 2004 <ccause+0x1042>
  a2ce4e:	b7e9                	j	a2ce18 <do_hard_fault_handler+0x2e>
  a2ce50:	8082                	ret

00a2ce52 <exc_info_display.part.2>:
  a2ce52:	8028                	push	{ra,s0},-16
  a2ce54:	842a                	mv	s0,a0
  a2ce56:	00a3 2b34 051f      	l.li	a0,0xa32b34
  a2ce5c:	9e8ff0ef          	jal	ra,a2c044 <print_str>
  a2ce60:	202e                	lhu	a1,2(s0)
  a2ce62:	00a3 2b6c 051f      	l.li	a0,0xa32b6c
  a2ce68:	9dcff0ef          	jal	ra,a2c044 <print_str>
  a2ce6c:	481c                	lw	a5,16(s0)
  a2ce6e:	00a3 2b84 051f      	l.li	a0,0xa32b84
  a2ce74:	4bcc                	lw	a1,20(a5)
  a2ce76:	9ceff0ef          	jal	ra,a2c044 <print_str>
  a2ce7a:	481c                	lw	a5,16(s0)
  a2ce7c:	00a3 2b9c 051f      	l.li	a0,0xa32b9c
  a2ce82:	4b8c                	lw	a1,16(a5)
  a2ce84:	9c0ff0ef          	jal	ra,a2c044 <print_str>
  a2ce88:	481c                	lw	a5,16(s0)
  a2ce8a:	00a3 2bb4 051f      	l.li	a0,0xa32bb4
  a2ce90:	478c                	lw	a1,8(a5)
  a2ce92:	9b2ff0ef          	jal	ra,a2c044 <print_str>
  a2ce96:	481c                	lw	a5,16(s0)
  a2ce98:	00a3 2bcc 051f      	l.li	a0,0xa32bcc
  a2ce9e:	43cc                	lw	a1,4(a5)
  a2cea0:	9a4ff0ef          	jal	ra,a2c044 <print_str>
  a2cea4:	481c                	lw	a5,16(s0)
  a2cea6:	00a3 2be4 051f      	l.li	a0,0xa32be4
  a2ceac:	438c                	lw	a1,0(a5)
  a2ceae:	996ff0ef          	jal	ra,a2c044 <print_str>
  a2ceb2:	481c                	lw	a5,16(s0)
  a2ceb4:	00a3 2bfc 051f      	l.li	a0,0xa32bfc
  a2ceba:	08c7a583          	lw	a1,140(a5)
  a2cebe:	986ff0ef          	jal	ra,a2c044 <print_str>
  a2cec2:	481c                	lw	a5,16(s0)
  a2cec4:	00a3 2c14 051f      	l.li	a0,0xa32c14
  a2ceca:	4fcc                	lw	a1,28(a5)
  a2cecc:	978ff0ef          	jal	ra,a2c044 <print_str>
  a2ced0:	481c                	lw	a5,16(s0)
  a2ced2:	00a3 2c2c 051f      	l.li	a0,0xa32c2c
  a2ced8:	47cc                	lw	a1,12(a5)
  a2ceda:	96aff0ef          	jal	ra,a2c044 <print_str>
  a2cede:	481c                	lw	a5,16(s0)
  a2cee0:	00a3 2c44 051f      	l.li	a0,0xa32c44
  a2cee6:	4f8c                	lw	a1,24(a5)
  a2cee8:	95cff0ef          	jal	ra,a2c044 <print_str>
  a2ceec:	481c                	lw	a5,16(s0)
  a2ceee:	00a3 2c5c 051f      	l.li	a0,0xa32c5c
  a2cef4:	0887a583          	lw	a1,136(a5)
  a2cef8:	94cff0ef          	jal	ra,a2c044 <print_str>
  a2cefc:	481c                	lw	a5,16(s0)
  a2cefe:	00a3 2c74 051f      	l.li	a0,0xa32c74
  a2cf04:	0847a583          	lw	a1,132(a5)
  a2cf08:	93cff0ef          	jal	ra,a2c044 <print_str>
  a2cf0c:	481c                	lw	a5,16(s0)
  a2cf0e:	00a3 2c8c 051f      	l.li	a0,0xa32c8c
  a2cf14:	0807a583          	lw	a1,128(a5)
  a2cf18:	92cff0ef          	jal	ra,a2c044 <print_str>
  a2cf1c:	481c                	lw	a5,16(s0)
  a2cf1e:	00a3 2ca4 051f      	l.li	a0,0xa32ca4
  a2cf24:	47ec                	lw	a1,76(a5)
  a2cf26:	91eff0ef          	jal	ra,a2c044 <print_str>
  a2cf2a:	481c                	lw	a5,16(s0)
  a2cf2c:	00a3 2cbc 051f      	l.li	a0,0xa32cbc
  a2cf32:	47ac                	lw	a1,72(a5)
  a2cf34:	910ff0ef          	jal	ra,a2c044 <print_str>
  a2cf38:	481c                	lw	a5,16(s0)
  a2cf3a:	00a3 2cd4 051f      	l.li	a0,0xa32cd4
  a2cf40:	5fec                	lw	a1,124(a5)
  a2cf42:	902ff0ef          	jal	ra,a2c044 <print_str>
  a2cf46:	481c                	lw	a5,16(s0)
  a2cf48:	00a3 2cec 051f      	l.li	a0,0xa32cec
  a2cf4e:	5fac                	lw	a1,120(a5)
  a2cf50:	8f4ff0ef          	jal	ra,a2c044 <print_str>
  a2cf54:	481c                	lw	a5,16(s0)
  a2cf56:	00a3 2d04 051f      	l.li	a0,0xa32d04
  a2cf5c:	5bec                	lw	a1,116(a5)
  a2cf5e:	8e6ff0ef          	jal	ra,a2c044 <print_str>
  a2cf62:	481c                	lw	a5,16(s0)
  a2cf64:	00a3 2d1c 051f      	l.li	a0,0xa32d1c
  a2cf6a:	5bac                	lw	a1,112(a5)
  a2cf6c:	8d8ff0ef          	jal	ra,a2c044 <print_str>
  a2cf70:	481c                	lw	a5,16(s0)
  a2cf72:	00a3 2d34 051f      	l.li	a0,0xa32d34
  a2cf78:	57ec                	lw	a1,108(a5)
  a2cf7a:	8caff0ef          	jal	ra,a2c044 <print_str>
  a2cf7e:	481c                	lw	a5,16(s0)
  a2cf80:	00a3 2d4c 051f      	l.li	a0,0xa32d4c
  a2cf86:	57ac                	lw	a1,104(a5)
  a2cf88:	8bcff0ef          	jal	ra,a2c044 <print_str>
  a2cf8c:	481c                	lw	a5,16(s0)
  a2cf8e:	00a3 2d64 051f      	l.li	a0,0xa32d64
  a2cf94:	53ec                	lw	a1,100(a5)
  a2cf96:	8aeff0ef          	jal	ra,a2c044 <print_str>
  a2cf9a:	481c                	lw	a5,16(s0)
  a2cf9c:	00a3 2d7c 051f      	l.li	a0,0xa32d7c
  a2cfa2:	53ac                	lw	a1,96(a5)
  a2cfa4:	8a0ff0ef          	jal	ra,a2c044 <print_str>
  a2cfa8:	481c                	lw	a5,16(s0)
  a2cfaa:	00a3 2d94 051f      	l.li	a0,0xa32d94
  a2cfb0:	43ec                	lw	a1,68(a5)
  a2cfb2:	892ff0ef          	jal	ra,a2c044 <print_str>
  a2cfb6:	481c                	lw	a5,16(s0)
  a2cfb8:	00a3 2dac 051f      	l.li	a0,0xa32dac
  a2cfbe:	43ac                	lw	a1,64(a5)
  a2cfc0:	884ff0ef          	jal	ra,a2c044 <print_str>
  a2cfc4:	481c                	lw	a5,16(s0)
  a2cfc6:	00a3 2dc4 051f      	l.li	a0,0xa32dc4
  a2cfcc:	5fcc                	lw	a1,60(a5)
  a2cfce:	876ff0ef          	jal	ra,a2c044 <print_str>
  a2cfd2:	481c                	lw	a5,16(s0)
  a2cfd4:	00a3 2ddc 051f      	l.li	a0,0xa32ddc
  a2cfda:	5f8c                	lw	a1,56(a5)
  a2cfdc:	868ff0ef          	jal	ra,a2c044 <print_str>
  a2cfe0:	481c                	lw	a5,16(s0)
  a2cfe2:	00a3 2df4 051f      	l.li	a0,0xa32df4
  a2cfe8:	5bcc                	lw	a1,52(a5)
  a2cfea:	85aff0ef          	jal	ra,a2c044 <print_str>
  a2cfee:	481c                	lw	a5,16(s0)
  a2cff0:	00a3 2e0c 051f      	l.li	a0,0xa32e0c
  a2cff6:	5b8c                	lw	a1,48(a5)
  a2cff8:	84cff0ef          	jal	ra,a2c044 <print_str>
  a2cffc:	481c                	lw	a5,16(s0)
  a2cffe:	00a3 2e24 051f      	l.li	a0,0xa32e24
  a2d004:	57cc                	lw	a1,44(a5)
  a2d006:	83eff0ef          	jal	ra,a2c044 <print_str>
  a2d00a:	481c                	lw	a5,16(s0)
  a2d00c:	00a3 2e3c 051f      	l.li	a0,0xa32e3c
  a2d012:	578c                	lw	a1,40(a5)
  a2d014:	830ff0ef          	jal	ra,a2c044 <print_str>
  a2d018:	481c                	lw	a5,16(s0)
  a2d01a:	00a3 2e54 051f      	l.li	a0,0xa32e54
  a2d020:	53cc                	lw	a1,36(a5)
  a2d022:	822ff0ef          	jal	ra,a2c044 <print_str>
  a2d026:	481c                	lw	a5,16(s0)
  a2d028:	00a3 2e6c 051f      	l.li	a0,0xa32e6c
  a2d02e:	538c                	lw	a1,32(a5)
  a2d030:	814ff0ef          	jal	ra,a2c044 <print_str>
  a2d034:	481c                	lw	a5,16(s0)
  a2d036:	00a3 2e84 051f      	l.li	a0,0xa32e84
  a2d03c:	4fec                	lw	a1,92(a5)
  a2d03e:	806ff0ef          	jal	ra,a2c044 <print_str>
  a2d042:	481c                	lw	a5,16(s0)
  a2d044:	00a3 2e9c 051f      	l.li	a0,0xa32e9c
  a2d04a:	4fac                	lw	a1,88(a5)
  a2d04c:	ff9fe0ef          	jal	ra,a2c044 <print_str>
  a2d050:	481c                	lw	a5,16(s0)
  a2d052:	00a3 2eb4 051f      	l.li	a0,0xa32eb4
  a2d058:	4bec                	lw	a1,84(a5)
  a2d05a:	febfe0ef          	jal	ra,a2c044 <print_str>
  a2d05e:	481c                	lw	a5,16(s0)
  a2d060:	00a3 2ecc 051f      	l.li	a0,0xa32ecc
  a2d066:	4bac                	lw	a1,80(a5)
  a2d068:	fddfe0ef          	jal	ra,a2c044 <print_str>
  a2d06c:	00a3 2ee4 051f      	l.li	a0,0xa32ee4
  a2d072:	8020                	pop	{ra,s0},16
  a2d074:	fd1fe06f          	j	a2c044 <print_str>

00a2d078 <do_process_exception>:
  a2d078:	4154                	lw	a3,4(a0)
  a2d07a:	00a067b7          	lui	a5,0xa06
  a2d07e:	84078713          	addi	a4,a5,-1984 # a05840 <g_exc_info>
  a2d082:	cb08                	sw	a0,16(a4)
  a2d084:	84078513          	addi	a0,a5,-1984
  a2d088:	a336                	sh	a3,2(a4)
  a2d08a:	dc9ff06f          	j	a2ce52 <exc_info_display.part.2>

00a2d08e <do_trap_unknown>:
  a2d08e:	8028                	push	{ra,s0},-16
  a2d090:	842a                	mv	s0,a0
  a2d092:	00a3 2b14 051f      	l.li	a0,0xa32b14
  a2d098:	fadfe0ef          	jal	ra,a2c044 <print_str>
  a2d09c:	8522                	mv	a0,s0
  a2d09e:	8020                	pop	{ra,s0},16
  a2d0a0:	fd9ff06f          	j	a2d078 <do_process_exception>

00a2d0a4 <do_trap_insn_misaligned>:
  a2d0a4:	8028                	push	{ra,s0},-16
  a2d0a6:	842a                	mv	s0,a0
  a2d0a8:	00a3 2a44 051f      	l.li	a0,0xa32a44
  a2d0ae:	f97fe0ef          	jal	ra,a2c044 <print_str>
  a2d0b2:	8522                	mv	a0,s0
  a2d0b4:	8020                	pop	{ra,s0},16
  a2d0b6:	fc3ff06f          	j	a2d078 <do_process_exception>

00a2d0ba <do_trap_insn_fault>:
  a2d0ba:	8028                	push	{ra,s0},-16
  a2d0bc:	842a                	mv	s0,a0
  a2d0be:	00a3 29f8 051f      	l.li	a0,0xa329f8
  a2d0c4:	f81fe0ef          	jal	ra,a2c044 <print_str>
  a2d0c8:	8522                	mv	a0,s0
  a2d0ca:	8020                	pop	{ra,s0},16
  a2d0cc:	fadff06f          	j	a2d078 <do_process_exception>

00a2d0d0 <do_trap_insn_illegal>:
  a2d0d0:	8028                	push	{ra,s0},-16
  a2d0d2:	842a                	mv	s0,a0
  a2d0d4:	00a3 2a20 051f      	l.li	a0,0xa32a20
  a2d0da:	f6bfe0ef          	jal	ra,a2c044 <print_str>
  a2d0de:	8522                	mv	a0,s0
  a2d0e0:	8020                	pop	{ra,s0},16
  a2d0e2:	f97ff06f          	j	a2d078 <do_process_exception>

00a2d0e6 <do_trap_load_misaligned>:
  a2d0e6:	8028                	push	{ra,s0},-16
  a2d0e8:	842a                	mv	s0,a0
  a2d0ea:	00a3 2a90 051f      	l.li	a0,0xa32a90
  a2d0f0:	f55fe0ef          	jal	ra,a2c044 <print_str>
  a2d0f4:	8522                	mv	a0,s0
  a2d0f6:	8020                	pop	{ra,s0},16
  a2d0f8:	f81ff06f          	j	a2d078 <do_process_exception>

00a2d0fc <do_trap_load_fault>:
  a2d0fc:	8028                	push	{ra,s0},-16
  a2d0fe:	842a                	mv	s0,a0
  a2d100:	00a3 2a70 051f      	l.li	a0,0xa32a70
  a2d106:	f3ffe0ef          	jal	ra,a2c044 <print_str>
  a2d10a:	8522                	mv	a0,s0
  a2d10c:	8020                	pop	{ra,s0},16
  a2d10e:	f6bff06f          	j	a2d078 <do_process_exception>

00a2d112 <do_trap_store_misaligned>:
  a2d112:	8028                	push	{ra,s0},-16
  a2d114:	842a                	mv	s0,a0
  a2d116:	00a3 2ae4 051f      	l.li	a0,0xa32ae4
  a2d11c:	f29fe0ef          	jal	ra,a2c044 <print_str>
  a2d120:	8522                	mv	a0,s0
  a2d122:	8020                	pop	{ra,s0},16
  a2d124:	f55ff06f          	j	a2d078 <do_process_exception>

00a2d128 <do_trap_store_fault>:
  a2d128:	8028                	push	{ra,s0},-16
  a2d12a:	842a                	mv	s0,a0
  a2d12c:	00a3 2ab8 051f      	l.li	a0,0xa32ab8
  a2d132:	f13fe0ef          	jal	ra,a2c044 <print_str>
  a2d136:	8522                	mv	a0,s0
  a2d138:	8020                	pop	{ra,s0},16
  a2d13a:	f3fff06f          	j	a2d078 <do_process_exception>

00a2d13e <do_trap_ecall_u>:
  a2d13e:	8028                	push	{ra,s0},-16
  a2d140:	842a                	mv	s0,a0
  a2d142:	00a3 29cc 051f      	l.li	a0,0xa329cc
  a2d148:	efdfe0ef          	jal	ra,a2c044 <print_str>
  a2d14c:	8522                	mv	a0,s0
  a2d14e:	8020                	pop	{ra,s0},16
  a2d150:	f29ff06f          	j	a2d078 <do_process_exception>

00a2d154 <do_trap_ecall_s>:
  a2d154:	8028                	push	{ra,s0},-16
  a2d156:	842a                	mv	s0,a0
  a2d158:	00a3 29a0 051f      	l.li	a0,0xa329a0
  a2d15e:	ee7fe0ef          	jal	ra,a2c044 <print_str>
  a2d162:	8522                	mv	a0,s0
  a2d164:	8020                	pop	{ra,s0},16
  a2d166:	f13ff06f          	j	a2d078 <do_process_exception>

00a2d16a <do_trap_ecall_m>:
  a2d16a:	8028                	push	{ra,s0},-16
  a2d16c:	842a                	mv	s0,a0
  a2d16e:	00a3 2974 051f      	l.li	a0,0xa32974
  a2d174:	ed1fe0ef          	jal	ra,a2c044 <print_str>
  a2d178:	8522                	mv	a0,s0
  a2d17a:	8020                	pop	{ra,s0},16
  a2d17c:	efdff06f          	j	a2d078 <do_process_exception>

00a2d180 <do_trap_break>:
  a2d180:	8028                	push	{ra,s0},-16
  a2d182:	842a                	mv	s0,a0
  a2d184:	00a3 2960 051f      	l.li	a0,0xa32960
  a2d18a:	ebbfe0ef          	jal	ra,a2c044 <print_str>
  a2d18e:	8522                	mv	a0,s0
  a2d190:	8020                	pop	{ra,s0},16
  a2d192:	ee7ff06f          	j	a2d078 <do_process_exception>

00a2d196 <do_insn_page_fault>:
  a2d196:	8028                	push	{ra,s0},-16
  a2d198:	842a                	mv	s0,a0
  a2d19a:	00a3 28e8 051f      	l.li	a0,0xa328e8
  a2d1a0:	ea5fe0ef          	jal	ra,a2c044 <print_str>
  a2d1a4:	8522                	mv	a0,s0
  a2d1a6:	8020                	pop	{ra,s0},16
  a2d1a8:	ed1ff06f          	j	a2d078 <do_process_exception>

00a2d1ac <do_load_page_fault>:
  a2d1ac:	8028                	push	{ra,s0},-16
  a2d1ae:	842a                	mv	s0,a0
  a2d1b0:	00a3 2908 051f      	l.li	a0,0xa32908
  a2d1b6:	e8ffe0ef          	jal	ra,a2c044 <print_str>
  a2d1ba:	8522                	mv	a0,s0
  a2d1bc:	8020                	pop	{ra,s0},16
  a2d1be:	ebbff06f          	j	a2d078 <do_process_exception>

00a2d1c2 <do_store_page_fault>:
  a2d1c2:	8028                	push	{ra,s0},-16
  a2d1c4:	842a                	mv	s0,a0
  a2d1c6:	00a3 2940 051f      	l.li	a0,0xa32940
  a2d1cc:	e79fe0ef          	jal	ra,a2c044 <print_str>
  a2d1d0:	8522                	mv	a0,s0
  a2d1d2:	8020                	pop	{ra,s0},16
  a2d1d4:	ea5ff06f          	j	a2d078 <do_process_exception>

00a2d1d8 <do_hard_fault>:
  a2d1d8:	8028                	push	{ra,s0},-16
  a2d1da:	842a                	mv	s0,a0
  a2d1dc:	00a3 28ac 051f      	l.li	a0,0xa328ac
  a2d1e2:	e63fe0ef          	jal	ra,a2c044 <print_str>
  a2d1e6:	8522                	mv	a0,s0
  a2d1e8:	8020                	pop	{ra,s0},16
  a2d1ea:	e8fff06f          	j	a2d078 <do_process_exception>

00a2d1ee <do_lockup>:
  a2d1ee:	8028                	push	{ra,s0},-16
  a2d1f0:	842a                	mv	s0,a0
  a2d1f2:	00a3 2928 051f      	l.li	a0,0xa32928
  a2d1f8:	e4dfe0ef          	jal	ra,a2c044 <print_str>
  a2d1fc:	8522                	mv	a0,s0
  a2d1fe:	8020                	pop	{ra,s0},16
  a2d200:	e79ff06f          	j	a2d078 <do_process_exception>

00a2d204 <default_handler>:
  a2d204:	8018                	push	{ra},-16
  a2d206:	bcfff0ef          	jal	ra,a2cdd4 <interrupt_number_get>
  a2d20a:	85aa                	mv	a1,a0
  a2d20c:	00a3 2880 051f      	l.li	a0,0xa32880
  a2d212:	e33fe0ef          	jal	ra,a2c044 <print_str>
  a2d216:	a001                	j	a2d216 <default_handler+0x12>

00a2d218 <nmi_handler>:
  a2d218:	8018                	push	{ra},-16
  a2d21a:	bbbff0ef          	jal	ra,a2cdd4 <interrupt_number_get>
  a2d21e:	85aa                	mv	a1,a0
  a2d220:	00a3 2f20 051f      	l.li	a0,0xa32f20
  a2d226:	e1ffe0ef          	jal	ra,a2c044 <print_str>
  a2d22a:	a001                	j	a2d22a <nmi_handler+0x12>

00a2d22c <partition_get_addr>:
  a2d22c:	0020 0380 051f      	l.li	a0,0x200380
  a2d232:	8082                	ret

00a2d234 <partition_get_path_map>:
  a2d234:	00a0 5858 079f      	l.li	a5,0xa05858
  a2d23a:	c11c                	sw	a5,0(a0)
  a2d23c:	4501                	li	a0,0
  a2d23e:	8082                	ret

00a2d240 <upg_progress_callbck>:
  a2d240:	8128                	push	{ra,s0},-32
  a2d242:	842a                	mv	s0,a0
  a2d244:	00a3 2fc0 051f      	l.li	a0,0xa32fc0
  a2d24a:	809ff0ef          	jal	ra,a2ca52 <serial_puts>
  a2d24e:	640413bb          	bnei	s0,100,a2d25c <upg_progress_callbck+0x1c>
  a2d252:	00a3 2fd8 051f      	l.li	a0,0xa32fd8
  a2d258:	ffaff0ef          	jal	ra,a2ca52 <serial_puts>
  a2d25c:	47a9                	li	a5,10
  a2d25e:	02f45733          	divu	a4,s0,a5
  a2d262:	0068                	addi	a0,sp,12
  a2d264:	00010723          	sb	zero,14(sp)
  a2d268:	02f77733          	remu	a4,a4,a5
  a2d26c:	02f477b3          	remu	a5,s0,a5
  a2d270:	03070713          	addi	a4,a4,48 # 40000030 <_gp_+0x3f5cb874>
  a2d274:	00e10623          	sb	a4,12(sp)
  a2d278:	03078793          	addi	a5,a5,48
  a2d27c:	00f106a3          	sb	a5,13(sp)
  a2d280:	fd2ff0ef          	jal	ra,a2ca52 <serial_puts>
  a2d284:	00a3 2fdc 051f      	l.li	a0,0xa32fdc
  a2d28a:	fc8ff0ef          	jal	ra,a2ca52 <serial_puts>
  a2d28e:	640413bb          	bnei	s0,100,a2d29c <upg_progress_callbck+0x5c>
  a2d292:	00a3 3088 051f      	l.li	a0,0xa33088
  a2d298:	fbaff0ef          	jal	ra,a2ca52 <serial_puts>
  a2d29c:	8124                	popret	{ra,s0},32

00a2d29e <check_fota_msid>:
  a2d29e:	8138                	push	{ra,s0-s1},-32
  a2d2a0:	842a                	mv	s0,a0
  a2d2a2:	84ae                	mv	s1,a1
  a2d2a4:	4611                	li	a2,4
  a2d2a6:	0f800593          	li	a1,248
  a2d2aa:	0068                	addi	a0,sp,12
  a2d2ac:	c602                	sw	zero,12(sp)
  a2d2ae:	dbafd0ef          	jal	ra,a2a868 <uapi_efuse_read_buffer>
  a2d2b2:	c519                	beqz	a0,a2d2c0 <check_fota_msid+0x22>
  a2d2b4:	85aa                	mv	a1,a0
  a2d2b6:	00a3 2f48 051f      	l.li	a0,0xa32f48
  a2d2bc:	d89fe0ef          	jal	ra,a2c044 <print_str>
  a2d2c0:	47b2                	lw	a5,12(sp)
  a2d2c2:	8c3d                	xor	s0,s0,a5
  a2d2c4:	8c65                	and	s0,s0,s1
  a2d2c6:	c411                	beqz	s0,a2d2d2 <check_fota_msid+0x34>
  a2d2c8:	00a3 2f6c 051f      	l.li	a0,0xa32f6c
  a2d2ce:	d77fe0ef          	jal	ra,a2c044 <print_str>
  a2d2d2:	4501                	li	a0,0
  a2d2d4:	8134                	popret	{ra,s0-s1},32

00a2d2d6 <ws63_upg_putc>:
  a2d2d6:	f5cff06f          	j	a2ca32 <serial_putc>

00a2d2da <upg_free_port>:
  a2d2da:	8038                	push	{ra,s0-s1},-16
  a2d2dc:	84aa                	mv	s1,a0
  a2d2de:	11b040ef          	jal	ra,a31bf8 <global_interrupt_lock>
  a2d2e2:	842a                	mv	s0,a0
  a2d2e4:	8526                	mv	a0,s1
  a2d2e6:	decfc0ef          	jal	ra,a298d2 <free>
  a2d2ea:	8522                	mv	a0,s0
  a2d2ec:	8030                	pop	{ra,s0-s1},16
  a2d2ee:	1230406f          	j	a31c10 <global_interrupt_restore>

00a2d2f2 <upg_malloc_port>:
  a2d2f2:	8038                	push	{ra,s0-s1},-16
  a2d2f4:	842a                	mv	s0,a0
  a2d2f6:	103040ef          	jal	ra,a31bf8 <global_interrupt_lock>
  a2d2fa:	84aa                	mv	s1,a0
  a2d2fc:	8522                	mv	a0,s0
  a2d2fe:	d9efc0ef          	jal	ra,a2989c <malloc>
  a2d302:	842a                	mv	s0,a0
  a2d304:	8526                	mv	a0,s1
  a2d306:	10b040ef          	jal	ra,a31c10 <global_interrupt_restore>
  a2d30a:	8522                	mv	a0,s0
  a2d30c:	8034                	popret	{ra,s0-s1},16

00a2d30e <upg_get_ids_map>:
  a2d30e:	00a3 3f48 079f      	l.li	a5,0xa33f48
  a2d314:	c11c                	sw	a5,0(a0)
  a2d316:	450d                	li	a0,3
  a2d318:	8082                	ret

00a2d31a <upg_img_in_set>:
  a2d31a:	00a3 3f70 079f      	l.li	a5,0xa33f70
  a2d320:	43d4                	lw	a3,4(a5)
  a2d322:	ce89                	beqz	a3,a2d33c <upg_img_in_set+0x22>
  a2d324:	4398                	lw	a4,0(a5)
  a2d326:	4781                	li	a5,0
  a2d328:	0711                	addi	a4,a4,4
  a2d32a:	ffc72603          	lw	a2,-4(a4)
  a2d32e:	00a60763          	beq	a2,a0,a2d33c <upg_img_in_set+0x22>
  a2d332:	0785                	addi	a5,a5,1
  a2d334:	fef69ae3          	bne	a3,a5,a2d328 <upg_img_in_set+0xe>
  a2d338:	4501                	li	a0,0
  a2d33a:	8082                	ret
  a2d33c:	4505                	li	a0,1
  a2d33e:	8082                	ret

00a2d340 <upg_get_upgrade_flag_flash_start_addr>:
  a2d340:	8128                	push	{ra,s0},-32
  a2d342:	842a                	mv	s0,a0
  a2d344:	004c                	addi	a1,sp,4
  a2d346:	02100513          	li	a0,33
  a2d34a:	06b000ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2d34e:	e519                	bnez	a0,a2d35c <upg_get_upgrade_flag_flash_start_addr+0x1c>
  a2d350:	4732                	lw	a4,12(sp)
  a2d352:	47a2                	lw	a5,8(sp)
  a2d354:	97ba                	add	a5,a5,a4
  a2d356:	777d                	lui	a4,0xfffff
  a2d358:	97ba                	add	a5,a5,a4
  a2d35a:	c01c                	sw	a5,0(s0)
  a2d35c:	8124                	popret	{ra,s0},32

00a2d35e <upg_get_progress_status_start_addr>:
  a2d35e:	8138                	push	{ra,s0-s1},-32
  a2d360:	84aa                	mv	s1,a0
  a2d362:	842e                	mv	s0,a1
  a2d364:	02100513          	li	a0,33
  a2d368:	004c                	addi	a1,sp,4
  a2d36a:	04b000ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2d36e:	e909                	bnez	a0,a2d380 <upg_get_progress_status_start_addr+0x22>
  a2d370:	4732                	lw	a4,12(sp)
  a2d372:	47a2                	lw	a5,8(sp)
  a2d374:	97ba                	add	a5,a5,a4
  a2d376:	7775                	lui	a4,0xffffd
  a2d378:	97ba                	add	a5,a5,a4
  a2d37a:	c09c                	sw	a5,0(s1)
  a2d37c:	678d                	lui	a5,0x3
  a2d37e:	c01c                	sw	a5,0(s0)
  a2d380:	8134                	popret	{ra,s0-s1},32

00a2d382 <upg_get_fota_partiton_area_addr>:
  a2d382:	8138                	push	{ra,s0-s1},-32
  a2d384:	84aa                	mv	s1,a0
  a2d386:	842e                	mv	s0,a1
  a2d388:	02100513          	li	a0,33
  a2d38c:	004c                	addi	a1,sp,4
  a2d38e:	027000ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2d392:	e509                	bnez	a0,a2d39c <upg_get_fota_partiton_area_addr+0x1a>
  a2d394:	47a2                	lw	a5,8(sp)
  a2d396:	c09c                	sw	a5,0(s1)
  a2d398:	47b2                	lw	a5,12(sp)
  a2d39a:	c01c                	sw	a5,0(s0)
  a2d39c:	8134                	popret	{ra,s0-s1},32

00a2d39e <upg_reboot>:
  a2d39e:	8018                	push	{ra},-16
  a2d3a0:	1f400513          	li	a0,500
  a2d3a4:	ea1fc0ef          	jal	ra,a2a244 <uapi_tcxo_delay_ms>
  a2d3a8:	8010                	pop	{ra},16
  a2d3aa:	91dfd06f          	j	a2acc6 <hal_reboot_chip>

00a2d3ae <upg_progress_callback_register>:
  a2d3ae:	8018                	push	{ra},-16
  a2d3b0:	00a2 d240 051f      	l.li	a0,0xa2d240
  a2d3b6:	35b010ef          	jal	ra,a2ef10 <uapi_upg_register_progress_callback>
  a2d3ba:	c519                	beqz	a0,a2d3c8 <upg_progress_callback_register+0x1a>
  a2d3bc:	00a3 2f90 051f      	l.li	a0,0xa32f90
  a2d3c2:	8010                	pop	{ra},16
  a2d3c4:	c81fe06f          	j	a2c044 <print_str>
  a2d3c8:	8014                	popret	{ra},16

00a2d3ca <upg_watchdog_kick>:
  a2d3ca:	c0cfd06f          	j	a2a7d6 <uapi_watchdog_kick>

00a2d3ce <upg_get_flash_base_addr>:
  a2d3ce:	00200537          	lui	a0,0x200
  a2d3d2:	8082                	ret

00a2d3d4 <upg_get_flash_size>:
  a2d3d4:	00800537          	lui	a0,0x800
  a2d3d8:	8082                	ret

00a2d3da <upg_flash_read>:
  a2d3da:	8018                	push	{ra},-16
  a2d3dc:	87b2                	mv	a5,a2
  a2d3de:	862e                	mv	a2,a1
  a2d3e0:	85be                	mv	a1,a5
  a2d3e2:	9aeff0ef          	jal	ra,a2c590 <uapi_sfc_reg_read>
  a2d3e6:	00a03533          	snez	a0,a0
  a2d3ea:	40a00533          	neg	a0,a0
  a2d3ee:	8014                	popret	{ra},16

00a2d3f0 <upg_flash_erase>:
  a2d3f0:	8098                	push	{ra,s0-s7},-48
  a2d3f2:	00b50a33          	add	s4,a0,a1
  a2d3f6:	014a1793          	slli	a5,s4,0x14
  a2d3fa:	84aa                	mv	s1,a0
  a2d3fc:	89d2                	mv	s3,s4
  a2d3fe:	c791                	beqz	a5,a2d40a <upg_flash_erase+0x1a>
  a2d400:	79fd                	lui	s3,0xfffff
  a2d402:	013a79b3          	and	s3,s4,s3
  a2d406:	6785                	lui	a5,0x1
  a2d408:	99be                	add	s3,s3,a5
  a2d40a:	6505                	lui	a0,0x1
  a2d40c:	029000ef          	jal	ra,a2dc34 <upg_malloc>
  a2d410:	8aaa                	mv	s5,a0
  a2d412:	c909                	beqz	a0,a2d424 <upg_flash_erase+0x34>
  a2d414:	6505                	lui	a0,0x1
  a2d416:	01f000ef          	jal	ra,a2dc34 <upg_malloc>
  a2d41a:	8b2a                	mv	s6,a0
  a2d41c:	e901                	bnez	a0,a2d42c <upg_flash_erase+0x3c>
  a2d41e:	8556                	mv	a0,s5
  a2d420:	02f000ef          	jal	ra,a2dc4e <upg_free>
  a2d424:	80000437          	lui	s0,0x80000
  a2d428:	0415                	addi	s0,s0,5 # 80000005 <_gp_+0x7f5cb849>
  a2d42a:	a881                	j	a2d47a <upg_flash_erase+0x8a>
  a2d42c:	797d                	lui	s2,0xfffff
  a2d42e:	0124f933          	and	s2,s1,s2
  a2d432:	412484b3          	sub	s1,s1,s2
  a2d436:	e48d                	bnez	s1,a2d460 <upg_flash_erase+0x70>
  a2d438:	41498bb3          	sub	s7,s3,s4
  a2d43c:	040b9163          	bnez	s7,a2d47e <upg_flash_erase+0x8e>
  a2d440:	412985b3          	sub	a1,s3,s2
  a2d444:	854a                	mv	a0,s2
  a2d446:	b1cff0ef          	jal	ra,a2c762 <uapi_sfc_reg_erase>
  a2d44a:	842a                	mv	s0,a0
  a2d44c:	e10d                	bnez	a0,a2d46e <upg_flash_erase+0x7e>
  a2d44e:	e0a1                	bnez	s1,a2d48e <upg_flash_erase+0x9e>
  a2d450:	000b8f63          	beqz	s7,a2d46e <upg_flash_erase+0x7e>
  a2d454:	4681                	li	a3,0
  a2d456:	865a                	mv	a2,s6
  a2d458:	85de                	mv	a1,s7
  a2d45a:	8552                	mv	a0,s4
  a2d45c:	2089                	jal	ra,a2d49e <upg_flash_write>
  a2d45e:	a835                	j	a2d49a <upg_flash_erase+0xaa>
  a2d460:	8656                	mv	a2,s5
  a2d462:	85a6                	mv	a1,s1
  a2d464:	854a                	mv	a0,s2
  a2d466:	f75ff0ef          	jal	ra,a2d3da <upg_flash_read>
  a2d46a:	842a                	mv	s0,a0
  a2d46c:	d571                	beqz	a0,a2d438 <upg_flash_erase+0x48>
  a2d46e:	8556                	mv	a0,s5
  a2d470:	7de000ef          	jal	ra,a2dc4e <upg_free>
  a2d474:	855a                	mv	a0,s6
  a2d476:	7d8000ef          	jal	ra,a2dc4e <upg_free>
  a2d47a:	8522                	mv	a0,s0
  a2d47c:	8094                	popret	{ra,s0-s7},48
  a2d47e:	865a                	mv	a2,s6
  a2d480:	85de                	mv	a1,s7
  a2d482:	8552                	mv	a0,s4
  a2d484:	f57ff0ef          	jal	ra,a2d3da <upg_flash_read>
  a2d488:	842a                	mv	s0,a0
  a2d48a:	f175                	bnez	a0,a2d46e <upg_flash_erase+0x7e>
  a2d48c:	bf55                	j	a2d440 <upg_flash_erase+0x50>
  a2d48e:	4681                	li	a3,0
  a2d490:	8656                	mv	a2,s5
  a2d492:	85a6                	mv	a1,s1
  a2d494:	854a                	mv	a0,s2
  a2d496:	2021                	jal	ra,a2d49e <upg_flash_write>
  a2d498:	dd45                	beqz	a0,a2d450 <upg_flash_erase+0x60>
  a2d49a:	842a                	mv	s0,a0
  a2d49c:	bfc9                	j	a2d46e <upg_flash_erase+0x7e>

00a2d49e <upg_flash_write>:
  a2d49e:	8068                	push	{ra,s0-s4},-32
  a2d4a0:	892a                	mv	s2,a0
  a2d4a2:	89ae                	mv	s3,a1
  a2d4a4:	8a32                	mv	s4,a2
  a2d4a6:	ee99                	bnez	a3,a2d4c4 <upg_flash_write+0x26>
  a2d4a8:	c2cfc0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2d4ac:	84aa                	mv	s1,a0
  a2d4ae:	864e                	mv	a2,s3
  a2d4b0:	85d2                	mv	a1,s4
  a2d4b2:	854a                	mv	a0,s2
  a2d4b4:	9b0ff0ef          	jal	ra,a2c664 <uapi_sfc_reg_write>
  a2d4b8:	842a                	mv	s0,a0
  a2d4ba:	8526                	mv	a0,s1
  a2d4bc:	c20fc0ef          	jal	ra,a298dc <osal_irq_restore>
  a2d4c0:	8522                	mv	a0,s0
  a2d4c2:	8064                	popret	{ra,s0-s4},32
  a2d4c4:	f2dff0ef          	jal	ra,a2d3f0 <upg_flash_erase>
  a2d4c8:	842a                	mv	s0,a0
  a2d4ca:	dd79                	beqz	a0,a2d4a8 <upg_flash_write+0xa>
  a2d4cc:	bfd5                	j	a2d4c0 <upg_flash_write+0x22>

00a2d4ce <upg_get_root_public_key>:
  a2d4ce:	8118                	push	{ra},-32
  a2d4d0:	004c                	addi	a1,sp,4
  a2d4d2:	450d                	li	a0,3
  a2d4d4:	6e0000ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2d4d8:	4781                	li	a5,0
  a2d4da:	e519                	bnez	a0,a2d4e8 <upg_get_root_public_key+0x1a>
  a2d4dc:	4722                	lw	a4,8(sp)
  a2d4de:	002007b7          	lui	a5,0x200
  a2d4e2:	97ba                	add	a5,a5,a4
  a2d4e4:	04078793          	addi	a5,a5,64 # 200040 <__heap_size+0x1e2a48>
  a2d4e8:	853e                	mv	a0,a5
  a2d4ea:	8114                	popret	{ra},32

00a2d4ec <upg_check_fota_information>:
  a2d4ec:	8028                	push	{ra,s0},-16
  a2d4ee:	842a                	mv	s0,a0
  a2d4f0:	590c                	lw	a1,48(a0)
  a2d4f2:	5548                	lw	a0,44(a0)
  a2d4f4:	dabff0ef          	jal	ra,a2d29e <check_fota_msid>
  a2d4f8:	e901                	bnez	a0,a2d508 <upg_check_fota_information+0x1c>
  a2d4fa:	11c42583          	lw	a1,284(s0)
  a2d4fe:	11842503          	lw	a0,280(s0)
  a2d502:	8020                	pop	{ra,s0},16
  a2d504:	d9bff06f          	j	a2d29e <check_fota_msid>
  a2d508:	8024                	popret	{ra,s0},16

00a2d50a <ws63_upg_init>:
  a2d50a:	8128                	push	{ra,s0},-32
  a2d50c:	00a2 d2f2 079f      	l.li	a5,0xa2d2f2
  a2d512:	c23e                	sw	a5,4(sp)
  a2d514:	00a2 d2da 079f      	l.li	a5,0xa2d2da
  a2d51a:	c43e                	sw	a5,8(sp)
  a2d51c:	0048                	addi	a0,sp,4
  a2d51e:	00a2 d2d6 079f      	l.li	a5,0xa2d2d6
  a2d524:	c63e                	sw	a5,12(sp)
  a2d526:	499000ef          	jal	ra,a2e1be <uapi_upg_init>
  a2d52a:	842a                	mv	s0,a0
  a2d52c:	c901                	beqz	a0,a2d53c <ws63_upg_init+0x32>
  a2d52e:	00a3 2fe0 051f      	l.li	a0,0xa32fe0
  a2d534:	b11fe0ef          	jal	ra,a2c044 <print_str>
  a2d538:	8522                	mv	a0,s0
  a2d53a:	8124                	popret	{ra,s0},32
  a2d53c:	e73ff0ef          	jal	ra,a2d3ae <upg_progress_callback_register>
  a2d540:	00a3 3000 051f      	l.li	a0,0xa33000
  a2d546:	b7fd                	j	a2d534 <ws63_upg_init+0x2a>

00a2d548 <upg_erase_whole_image>:
  a2d548:	8118                	push	{ra},-32
  a2d54a:	4148                	lw	a0,4(a0)
  a2d54c:	004c                	addi	a1,sp,4
  a2d54e:	79a000ef          	jal	ra,a2dce8 <upg_get_image_info>
  a2d552:	e509                	bnez	a0,a2d55c <upg_erase_whole_image+0x14>
  a2d554:	45b2                	lw	a1,12(sp)
  a2d556:	4522                	lw	a0,8(sp)
  a2d558:	e99ff0ef          	jal	ra,a2d3f0 <upg_flash_erase>
  a2d55c:	8114                	popret	{ra},32

00a2d55e <upg_cryto_init>:
  a2d55e:	8148                	push	{ra,s0-s2},-32
  a2d560:	47f1                	li	a5,28
  a2d562:	5178                	lw	a4,100(a0)
  a2d564:	06c50913          	addi	s2,a0,108 # 106c <ccause+0xaa>
  a2d568:	c43e                	sw	a5,8(sp)
  a2d56a:	4785                	li	a5,1
  a2d56c:	00f10623          	sb	a5,12(sp)
  a2d570:	00010023          	sb	zero,0(sp)
  a2d574:	000100a3          	sb	zero,1(sp)
  a2d578:	00010123          	sb	zero,2(sp)
  a2d57c:	c24a                	sw	s2,4(sp)
  a2d57e:	3c78 96e1 079f      	l.li	a5,0x3c7896e1
  a2d584:	4401                	li	s0,0
  a2d586:	02f71d63          	bne	a4,a5,a2d5c0 <upg_cryto_init+0x62>
  a2d58a:	00a0 5858 049f      	l.li	s1,0xa05858
  a2d590:	030007b7          	lui	a5,0x3000
  a2d594:	00d78713          	addi	a4,a5,13 # 300000d <_gp_+0x25cb851>
  a2d598:	842a                	mv	s0,a0
  a2d59a:	07cd                	addi	a5,a5,19
  a2d59c:	46f1                	li	a3,28
  a2d59e:	864a                	mv	a2,s2
  a2d5a0:	45f1                	li	a1,28
  a2d5a2:	03c48513          	addi	a0,s1,60
  a2d5a6:	d8d8                	sw	a4,52(s1)
  a2d5a8:	c09c                	sw	a5,0(s1)
  a2d5aa:	3c6030ef          	jal	ra,a30970 <memcpy_s>
  a2d5ae:	c919                	beqz	a0,a2d5c4 <upg_cryto_init+0x66>
  a2d5b0:	00a3 301c 051f      	l.li	a0,0xa3301c
  a2d5b6:	80000437          	lui	s0,0x80000
  a2d5ba:	a8bfe0ef          	jal	ra,a2c044 <print_str>
  a2d5be:	0411                	addi	s0,s0,4 # 80000004 <_gp_+0x7f5cb848>
  a2d5c0:	8522                	mv	a0,s0
  a2d5c2:	8144                	popret	{ra,s0-s2},32
  a2d5c4:	46f1                	li	a3,28
  a2d5c6:	864a                	mv	a2,s2
  a2d5c8:	45f1                	li	a1,28
  a2d5ca:	00848513          	addi	a0,s1,8
  a2d5ce:	3a2030ef          	jal	ra,a30970 <memcpy_s>
  a2d5d2:	c509                	beqz	a0,a2d5dc <upg_cryto_init+0x7e>
  a2d5d4:	00a3 3054 051f      	l.li	a0,0xa33054
  a2d5da:	bff1                	j	a2d5b6 <upg_cryto_init+0x58>
  a2d5dc:	08c40413          	addi	s0,s0,140
  a2d5e0:	46c1                	li	a3,16
  a2d5e2:	8622                	mv	a2,s0
  a2d5e4:	45c1                	li	a1,16
  a2d5e6:	05848513          	addi	a0,s1,88
  a2d5ea:	386030ef          	jal	ra,a30970 <memcpy_s>
  a2d5ee:	c509                	beqz	a0,a2d5f8 <upg_cryto_init+0x9a>
  a2d5f0:	00a3 308c 051f      	l.li	a0,0xa3308c
  a2d5f6:	b7c1                	j	a2d5b6 <upg_cryto_init+0x58>
  a2d5f8:	46c1                	li	a3,16
  a2d5fa:	8622                	mv	a2,s0
  a2d5fc:	45c1                	li	a1,16
  a2d5fe:	02448513          	addi	a0,s1,36
  a2d602:	36e030ef          	jal	ra,a30970 <memcpy_s>
  a2d606:	c509                	beqz	a0,a2d610 <upg_cryto_init+0xb2>
  a2d608:	00a3 30c0 051f      	l.li	a0,0xa330c0
  a2d60e:	b765                	j	a2d5b6 <upg_cryto_init+0x58>
  a2d610:	ee8fc0ef          	jal	ra,a29cf8 <drv_rom_cipher_symc_init>
  a2d614:	842a                	mv	s0,a0
  a2d616:	c901                	beqz	a0,a2d626 <upg_cryto_init+0xc8>
  a2d618:	85aa                	mv	a1,a0
  a2d61a:	00a3 30f4 051f      	l.li	a0,0xa330f4
  a2d620:	a25fe0ef          	jal	ra,a2c044 <print_str>
  a2d624:	bf71                	j	a2d5c0 <upg_cryto_init+0x62>
  a2d626:	58d0                	lw	a2,52(s1)
  a2d628:	858a                	mv	a1,sp
  a2d62a:	03848513          	addi	a0,s1,56
  a2d62e:	c74fc0ef          	jal	ra,a29aa2 <drv_rom_cipher_create_keyslot>
  a2d632:	842a                	mv	s0,a0
  a2d634:	c911                	beqz	a0,a2d648 <upg_cryto_init+0xea>
  a2d636:	85aa                	mv	a1,a0
  a2d638:	00a3 3128 051f      	l.li	a0,0xa33128
  a2d63e:	a07fe0ef          	jal	ra,a2c044 <print_str>
  a2d642:	f06fc0ef          	jal	ra,a29d48 <drv_rom_cipher_symc_deinit>
  a2d646:	bfad                	j	a2d5c0 <upg_cryto_init+0x62>
  a2d648:	4090                	lw	a2,0(s1)
  a2d64a:	858a                	mv	a1,sp
  a2d64c:	00448513          	addi	a0,s1,4
  a2d650:	c52fc0ef          	jal	ra,a29aa2 <drv_rom_cipher_create_keyslot>
  a2d654:	842a                	mv	s0,a0
  a2d656:	d52d                	beqz	a0,a2d5c0 <upg_cryto_init+0x62>
  a2d658:	85aa                	mv	a1,a0
  a2d65a:	00a3 316c 051f      	l.li	a0,0xa3316c
  a2d660:	9e5fe0ef          	jal	ra,a2c044 <print_str>
  a2d664:	5c88                	lw	a0,56(s1)
  a2d666:	cb8fc0ef          	jal	ra,a29b1e <drv_rom_cipher_destroy_keyslot>
  a2d66a:	bfe1                	j	a2d642 <upg_cryto_init+0xe4>

00a2d66c <upg_cryto_deinit>:
  a2d66c:	5178                	lw	a4,100(a0)
  a2d66e:	3c78 96e1 079f      	l.li	a5,0x3c7896e1
  a2d674:	00f70463          	beq	a4,a5,a2d67c <upg_cryto_deinit+0x10>
  a2d678:	4501                	li	a0,0
  a2d67a:	8082                	ret
  a2d67c:	8038                	push	{ra,s0-s1},-16
  a2d67e:	00a0 5858 049f      	l.li	s1,0xa05858
  a2d684:	5c88                	lw	a0,56(s1)
  a2d686:	c98fc0ef          	jal	ra,a29b1e <drv_rom_cipher_destroy_keyslot>
  a2d68a:	842a                	mv	s0,a0
  a2d68c:	40c8                	lw	a0,4(s1)
  a2d68e:	c90fc0ef          	jal	ra,a29b1e <drv_rom_cipher_destroy_keyslot>
  a2d692:	8c49                	or	s0,s0,a0
  a2d694:	eb4fc0ef          	jal	ra,a29d48 <drv_rom_cipher_symc_deinit>
  a2d698:	8d41                	or	a0,a0,s0
  a2d69a:	8034                	popret	{ra,s0-s1},16

00a2d69c <upg_process_cryto_info>:
  a2d69c:	219c                	lbu	a5,0(a1)
  a2d69e:	e789                	bnez	a5,a2d6a8 <upg_process_cryto_info+0xc>
  a2d6a0:	00052023          	sw	zero,0(a0)
  a2d6a4:	4501                	li	a0,0
  a2d6a6:	8082                	ret
  a2d6a8:	30000793          	li	a5,768
  a2d6ac:	c11c                	sw	a5,0(a0)
  a2d6ae:	00058023          	sb	zero,0(a1)
  a2d6b2:	bfcd                	j	a2d6a4 <upg_process_cryto_info+0x8>

00a2d6b4 <upg_decry_fota_pkt>:
  a2d6b4:	8028                	push	{ra,s0},-16
  a2d6b6:	5278                	lw	a4,100(a2)
  a2d6b8:	3c78 96e1 079f      	l.li	a5,0x3c7896e1
  a2d6be:	4401                	li	s0,0
  a2d6c0:	02f71863          	bne	a4,a5,a2d6f0 <upg_decry_fota_pkt+0x3c>
  a2d6c4:	87aa                	mv	a5,a0
  a2d6c6:	00a0 5858 051f      	l.li	a0,0xa05858
  a2d6cc:	88ae                	mv	a7,a1
  a2d6ce:	02450593          	addi	a1,a0,36
  a2d6d2:	4148                	lw	a0,4(a0)
  a2d6d4:	4801                	li	a6,0
  a2d6d6:	86be                	mv	a3,a5
  a2d6d8:	4701                	li	a4,0
  a2d6da:	4641                	li	a2,16
  a2d6dc:	e7cfc0ef          	jal	ra,a29d58 <drv_rom_cipher_symc_decrypt>
  a2d6e0:	842a                	mv	s0,a0
  a2d6e2:	c519                	beqz	a0,a2d6f0 <upg_decry_fota_pkt+0x3c>
  a2d6e4:	85aa                	mv	a1,a0
  a2d6e6:	00a3 31b0 051f      	l.li	a0,0xa331b0
  a2d6ec:	959fe0ef          	jal	ra,a2c044 <print_str>
  a2d6f0:	8522                	mv	a0,s0
  a2d6f2:	8024                	popret	{ra,s0},16

00a2d6f4 <upg_encry_fota_pkt>:
  a2d6f4:	8028                	push	{ra,s0},-16
  a2d6f6:	5278                	lw	a4,100(a2)
  a2d6f8:	3c78 96e1 079f      	l.li	a5,0x3c7896e1
  a2d6fe:	4401                	li	s0,0
  a2d700:	02f71863          	bne	a4,a5,a2d730 <upg_encry_fota_pkt+0x3c>
  a2d704:	87aa                	mv	a5,a0
  a2d706:	00a0 5858 051f      	l.li	a0,0xa05858
  a2d70c:	88ae                	mv	a7,a1
  a2d70e:	05850593          	addi	a1,a0,88
  a2d712:	5d08                	lw	a0,56(a0)
  a2d714:	4801                	li	a6,0
  a2d716:	86be                	mv	a3,a5
  a2d718:	4701                	li	a4,0
  a2d71a:	4641                	li	a2,16
  a2d71c:	e3cfc0ef          	jal	ra,a29d58 <drv_rom_cipher_symc_decrypt>
  a2d720:	842a                	mv	s0,a0
  a2d722:	c519                	beqz	a0,a2d730 <upg_encry_fota_pkt+0x3c>
  a2d724:	85aa                	mv	a1,a0
  a2d726:	00a3 31fc 051f      	l.li	a0,0xa331fc
  a2d72c:	919fe0ef          	jal	ra,a2c044 <print_str>
  a2d730:	8522                	mv	a0,s0
  a2d732:	8024                	popret	{ra,s0},16

00a2d734 <mfg_malloc>:
  a2d734:	8038                	push	{ra,s0-s1},-16
  a2d736:	842a                	mv	s0,a0
  a2d738:	4c0040ef          	jal	ra,a31bf8 <global_interrupt_lock>
  a2d73c:	84aa                	mv	s1,a0
  a2d73e:	8522                	mv	a0,s0
  a2d740:	95cfc0ef          	jal	ra,a2989c <malloc>
  a2d744:	842a                	mv	s0,a0
  a2d746:	8526                	mv	a0,s1
  a2d748:	4c8040ef          	jal	ra,a31c10 <global_interrupt_restore>
  a2d74c:	8522                	mv	a0,s0
  a2d74e:	8034                	popret	{ra,s0-s1},16

00a2d750 <mfg_free>:
  a2d750:	8038                	push	{ra,s0-s1},-16
  a2d752:	84aa                	mv	s1,a0
  a2d754:	4a4040ef          	jal	ra,a31bf8 <global_interrupt_lock>
  a2d758:	842a                	mv	s0,a0
  a2d75a:	8526                	mv	a0,s1
  a2d75c:	976fc0ef          	jal	ra,a298d2 <free>
  a2d760:	8522                	mv	a0,s0
  a2d762:	8030                	pop	{ra,s0-s1},16
  a2d764:	4ac0406f          	j	a31c10 <global_interrupt_restore>

00a2d768 <mfg_flash_read>:
  a2d768:	8138                	push	{ra,s0-s1},-32
  a2d76a:	84aa                	mv	s1,a0
  a2d76c:	842e                	mv	s0,a1
  a2d76e:	46b1                	li	a3,12
  a2d770:	45b1                	li	a1,12
  a2d772:	4601                	li	a2,0
  a2d774:	0048                	addi	a0,sp,4
  a2d776:	276030ef          	jal	ra,a309ec <memset_s>
  a2d77a:	004c                	addi	a1,sp,4
  a2d77c:	02100513          	li	a0,33
  a2d780:	2915                	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2d782:	e505                	bnez	a0,a2d7aa <mfg_flash_read+0x42>
  a2d784:	4532                	lw	a0,12(sp)
  a2d786:	47a2                	lw	a5,8(sp)
  a2d788:	004006b7          	lui	a3,0x400
  a2d78c:	97aa                	add	a5,a5,a0
  a2d78e:	7579                	lui	a0,0xffffe
  a2d790:	97aa                	add	a5,a5,a0
  a2d792:	557d                	li	a0,-1
  a2d794:	00f6eb63          	bltu	a3,a5,a2d7aa <mfg_flash_read+0x42>
  a2d798:	00878733          	add	a4,a5,s0
  a2d79c:	00e6e763          	bltu	a3,a4,a2d7aa <mfg_flash_read+0x42>
  a2d7a0:	8622                	mv	a2,s0
  a2d7a2:	85a6                	mv	a1,s1
  a2d7a4:	853e                	mv	a0,a5
  a2d7a6:	debfe0ef          	jal	ra,a2c590 <uapi_sfc_reg_read>
  a2d7aa:	8134                	popret	{ra,s0-s1},32

00a2d7ac <mfg_flash_write_info>:
  a2d7ac:	80c8                	push	{ra,s0-s10},-48
  a2d7ae:	8baa                	mv	s7,a0
  a2d7b0:	8aae                	mv	s5,a1
  a2d7b2:	89b2                	mv	s3,a2
  a2d7b4:	ea99                	bnez	a3,a2d7ca <mfg_flash_write_info+0x1e>
  a2d7b6:	854e                	mv	a0,s3
  a2d7b8:	f7dff0ef          	jal	ra,a2d734 <mfg_malloc>
  a2d7bc:	84aa                	mv	s1,a0
  a2d7be:	e955                	bnez	a0,a2d872 <mfg_flash_write_info+0xc6>
  a2d7c0:	80000437          	lui	s0,0x80000
  a2d7c4:	0415                	addi	s0,s0,5 # 80000005 <_gp_+0x7f5cb849>
  a2d7c6:	8522                	mv	a0,s0
  a2d7c8:	80c4                	popret	{ra,s0-s10},48
  a2d7ca:	00c58a33          	add	s4,a1,a2
  a2d7ce:	014a1793          	slli	a5,s4,0x14
  a2d7d2:	8952                	mv	s2,s4
  a2d7d4:	c791                	beqz	a5,a2d7e0 <mfg_flash_write_info+0x34>
  a2d7d6:	797d                	lui	s2,0xfffff
  a2d7d8:	012a7933          	and	s2,s4,s2
  a2d7dc:	6785                	lui	a5,0x1
  a2d7de:	993e                	add	s2,s2,a5
  a2d7e0:	6505                	lui	a0,0x1
  a2d7e2:	f53ff0ef          	jal	ra,a2d734 <mfg_malloc>
  a2d7e6:	8b2a                	mv	s6,a0
  a2d7e8:	dd61                	beqz	a0,a2d7c0 <mfg_flash_write_info+0x14>
  a2d7ea:	6505                	lui	a0,0x1
  a2d7ec:	f49ff0ef          	jal	ra,a2d734 <mfg_malloc>
  a2d7f0:	8c2a                	mv	s8,a0
  a2d7f2:	e509                	bnez	a0,a2d7fc <mfg_flash_write_info+0x50>
  a2d7f4:	855a                	mv	a0,s6
  a2d7f6:	f5bff0ef          	jal	ra,a2d750 <mfg_free>
  a2d7fa:	b7d9                	j	a2d7c0 <mfg_flash_write_info+0x14>
  a2d7fc:	74fd                	lui	s1,0xfffff
  a2d7fe:	009af4b3          	and	s1,s5,s1
  a2d802:	409a8d33          	sub	s10,s5,s1
  a2d806:	020d1763          	bnez	s10,a2d834 <mfg_flash_write_info+0x88>
  a2d80a:	41490cb3          	sub	s9,s2,s4
  a2d80e:	040c9263          	bnez	s9,a2d852 <mfg_flash_write_info+0xa6>
  a2d812:	409905b3          	sub	a1,s2,s1
  a2d816:	8526                	mv	a0,s1
  a2d818:	f4bfe0ef          	jal	ra,a2c762 <uapi_sfc_reg_erase>
  a2d81c:	842a                	mv	s0,a0
  a2d81e:	e115                	bnez	a0,a2d842 <mfg_flash_write_info+0x96>
  a2d820:	040d1163          	bnez	s10,a2d862 <mfg_flash_write_info+0xb6>
  a2d824:	000c8f63          	beqz	s9,a2d842 <mfg_flash_write_info+0x96>
  a2d828:	4681                	li	a3,0
  a2d82a:	8666                	mv	a2,s9
  a2d82c:	85d2                	mv	a1,s4
  a2d82e:	8562                	mv	a0,s8
  a2d830:	3fb5                	jal	ra,a2d7ac <mfg_flash_write_info>
  a2d832:	a835                	j	a2d86e <mfg_flash_write_info+0xc2>
  a2d834:	866a                	mv	a2,s10
  a2d836:	85da                	mv	a1,s6
  a2d838:	8526                	mv	a0,s1
  a2d83a:	d57fe0ef          	jal	ra,a2c590 <uapi_sfc_reg_read>
  a2d83e:	842a                	mv	s0,a0
  a2d840:	d569                	beqz	a0,a2d80a <mfg_flash_write_info+0x5e>
  a2d842:	855a                	mv	a0,s6
  a2d844:	f0dff0ef          	jal	ra,a2d750 <mfg_free>
  a2d848:	8562                	mv	a0,s8
  a2d84a:	f07ff0ef          	jal	ra,a2d750 <mfg_free>
  a2d84e:	d425                	beqz	s0,a2d7b6 <mfg_flash_write_info+0xa>
  a2d850:	bf9d                	j	a2d7c6 <mfg_flash_write_info+0x1a>
  a2d852:	8666                	mv	a2,s9
  a2d854:	85e2                	mv	a1,s8
  a2d856:	8552                	mv	a0,s4
  a2d858:	d39fe0ef          	jal	ra,a2c590 <uapi_sfc_reg_read>
  a2d85c:	842a                	mv	s0,a0
  a2d85e:	f175                	bnez	a0,a2d842 <mfg_flash_write_info+0x96>
  a2d860:	bf4d                	j	a2d812 <mfg_flash_write_info+0x66>
  a2d862:	4681                	li	a3,0
  a2d864:	866a                	mv	a2,s10
  a2d866:	85a6                	mv	a1,s1
  a2d868:	855a                	mv	a0,s6
  a2d86a:	3789                	jal	ra,a2d7ac <mfg_flash_write_info>
  a2d86c:	dd45                	beqz	a0,a2d824 <mfg_flash_write_info+0x78>
  a2d86e:	842a                	mv	s0,a0
  a2d870:	bfc9                	j	a2d842 <mfg_flash_write_info+0x96>
  a2d872:	864e                	mv	a2,s3
  a2d874:	85de                	mv	a1,s7
  a2d876:	8556                	mv	a0,s5
  a2d878:	dedfe0ef          	jal	ra,a2c664 <uapi_sfc_reg_write>
  a2d87c:	842a                	mv	s0,a0
  a2d87e:	e10d                	bnez	a0,a2d8a0 <mfg_flash_write_info+0xf4>
  a2d880:	00200637          	lui	a2,0x200
  a2d884:	86ce                	mv	a3,s3
  a2d886:	9656                	add	a2,a2,s5
  a2d888:	85ce                	mv	a1,s3
  a2d88a:	8526                	mv	a0,s1
  a2d88c:	0e4030ef          	jal	ra,a30970 <memcpy_s>
  a2d890:	842a                	mv	s0,a0
  a2d892:	e519                	bnez	a0,a2d8a0 <mfg_flash_write_info+0xf4>
  a2d894:	864e                	mv	a2,s3
  a2d896:	85de                	mv	a1,s7
  a2d898:	8526                	mv	a0,s1
  a2d89a:	c54fb0ef          	jal	ra,a28cee <memcmp>
  a2d89e:	842a                	mv	s0,a0
  a2d8a0:	8526                	mv	a0,s1
  a2d8a2:	eafff0ef          	jal	ra,a2d750 <mfg_free>
  a2d8a6:	b705                	j	a2d7c6 <mfg_flash_write_info+0x1a>

00a2d8a8 <mfg_flash_write>:
  a2d8a8:	8138                	push	{ra,s0-s1},-32
  a2d8aa:	84aa                	mv	s1,a0
  a2d8ac:	842e                	mv	s0,a1
  a2d8ae:	46b1                	li	a3,12
  a2d8b0:	45b1                	li	a1,12
  a2d8b2:	4601                	li	a2,0
  a2d8b4:	0048                	addi	a0,sp,4
  a2d8b6:	136030ef          	jal	ra,a309ec <memset_s>
  a2d8ba:	004c                	addi	a1,sp,4
  a2d8bc:	02100513          	li	a0,33
  a2d8c0:	2cd5                	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2d8c2:	e505                	bnez	a0,a2d8ea <mfg_flash_write+0x42>
  a2d8c4:	47b2                	lw	a5,12(sp)
  a2d8c6:	45a2                	lw	a1,8(sp)
  a2d8c8:	00400737          	lui	a4,0x400
  a2d8cc:	557d                	li	a0,-1
  a2d8ce:	95be                	add	a1,a1,a5
  a2d8d0:	77f9                	lui	a5,0xffffe
  a2d8d2:	95be                	add	a1,a1,a5
  a2d8d4:	00b76b63          	bltu	a4,a1,a2d8ea <mfg_flash_write+0x42>
  a2d8d8:	008587b3          	add	a5,a1,s0
  a2d8dc:	00f76763          	bltu	a4,a5,a2d8ea <mfg_flash_write+0x42>
  a2d8e0:	4685                	li	a3,1
  a2d8e2:	8622                	mv	a2,s0
  a2d8e4:	8526                	mv	a0,s1
  a2d8e6:	ec7ff0ef          	jal	ra,a2d7ac <mfg_flash_write_info>
  a2d8ea:	8134                	popret	{ra,s0-s1},32

00a2d8ec <mfg_get_ftm_run_region>:
  a2d8ec:	8328                	push	{ra,s0},-64
  a2d8ee:	c402                	sw	zero,8(sp)
  a2d8f0:	c602                	sw	zero,12(sp)
  a2d8f2:	c802                	sw	zero,16(sp)
  a2d8f4:	c92d                	beqz	a0,a2d966 <mfg_get_ftm_run_region+0x7a>
  a2d8f6:	842a                	mv	s0,a0
  a2d8f8:	002c                	addi	a1,sp,8
  a2d8fa:	02100513          	li	a0,33
  a2d8fe:	2c5d                	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2d900:	e13d                	bnez	a0,a2d966 <mfg_get_ftm_run_region+0x7a>
  a2d902:	45f1                	li	a1,28
  a2d904:	8522                	mv	a0,s0
  a2d906:	e63ff0ef          	jal	ra,a2d768 <mfg_flash_read>
  a2d90a:	ed31                	bnez	a0,a2d966 <mfg_get_ftm_run_region+0x7a>
  a2d90c:	4732                	lw	a4,12(sp)
  a2d90e:	4c14                	lw	a3,24(s0)
  a2d910:	002007b7          	lui	a5,0x200
  a2d914:	8f45 ba2f 061f      	l.li	a2,0x8f45ba2f
  a2d91a:	97ba                	add	a5,a5,a4
  a2d91c:	04c68763          	beq	a3,a2,a2d96a <mfg_get_ftm_run_region+0x7e>
  a2d920:	438c                	lw	a1,0(a5)
  a2d922:	4b7c f333 069f      	l.li	a3,0x4b7cf333
  a2d928:	04d59163          	bne	a1,a3,a2d96a <mfg_get_ftm_run_region+0x7e>
  a2d92c:	478c                	lw	a1,8(a5)
  a2d92e:	10000693          	li	a3,256
  a2d932:	02d59c63          	bne	a1,a3,a2d96a <mfg_get_ftm_run_region+0x7e>
  a2d936:	1247a683          	lw	a3,292(a5) # 200124 <__heap_size+0x1e2b2c>
  a2d93a:	4585                	li	a1,1
  a2d93c:	cc3e                	sw	a5,24(sp)
  a2d93e:	30068693          	addi	a3,a3,768 # 400300 <__heap_size+0x3e2d08>
  a2d942:	ce3e                	sw	a5,28(sp)
  a2d944:	ca2e                	sw	a1,20(sp)
  a2d946:	d036                	sw	a3,32(sp)
  a2d948:	d236                	sw	a3,36(sp)
  a2d94a:	d402                	sw	zero,40(sp)
  a2d94c:	d632                	sw	a2,44(sp)
  a2d94e:	004007b7          	lui	a5,0x400
  a2d952:	00e7ea63          	bltu	a5,a4,a2d966 <mfg_get_ftm_run_region+0x7a>
  a2d956:	9736                	add	a4,a4,a3
  a2d958:	00e7e763          	bltu	a5,a4,a2d966 <mfg_get_ftm_run_region+0x7a>
  a2d95c:	45f1                	li	a1,28
  a2d95e:	0848                	addi	a0,sp,20
  a2d960:	f49ff0ef          	jal	ra,a2d8a8 <mfg_flash_write>
  a2d964:	c119                	beqz	a0,a2d96a <mfg_get_ftm_run_region+0x7e>
  a2d966:	4501                	li	a0,0
  a2d968:	a809                	j	a2d97a <mfg_get_ftm_run_region+0x8e>
  a2d96a:	45f1                	li	a1,28
  a2d96c:	8522                	mv	a0,s0
  a2d96e:	dfbff0ef          	jal	ra,a2d768 <mfg_flash_read>
  a2d972:	f975                	bnez	a0,a2d966 <mfg_get_ftm_run_region+0x7a>
  a2d974:	4008                	lw	a0,0(s0)
  a2d976:	02f57c3b          	bgeui	a0,2,a2d966 <mfg_get_ftm_run_region+0x7a>
  a2d97a:	8324                	popret	{ra,s0},64

00a2d97c <cpu_utils_set_system_status_by_cause>:
  a2d97c:	8018                	push	{ra},-16
  a2d97e:	223d                	jal	ra,a2daac <set_cpu_utils_reset_cause>
  a2d980:	8010                	pop	{ra},16
  a2d982:	a8e9                	j	a2da5c <set_cpu_utils_system_boot_magic>

00a2d984 <cpu_utils_reset_chip_with_log>:
  a2d984:	8118                	push	{ra},-32
  a2d986:	4501                	li	a0,0
  a2d988:	c62e                	sw	a1,12(sp)
  a2d98a:	2a15                	jal	ra,a2dabe <set_update_reset_cause_on_boot>
  a2d98c:	45b2                	lw	a1,12(sp)
  a2d98e:	852e                	mv	a0,a1
  a2d990:	fedff0ef          	jal	ra,a2d97c <cpu_utils_set_system_status_by_cause>
  a2d994:	8110                	pop	{ra},32
  a2d996:	b30fd06f          	j	a2acc6 <hal_reboot_chip>

00a2d99a <panic_deal>:
  a2d99a:	8458                	push	{ra,s0-s3},-96
  a2d99c:	86b2                	mv	a3,a2
  a2d99e:	892a                	mv	s2,a0
  a2d9a0:	84ae                	mv	s1,a1
  a2d9a2:	8432                	mv	s0,a2
  a2d9a4:	862e                	mv	a2,a1
  a2d9a6:	85aa                	mv	a1,a0
  a2d9a8:	00a3 3248 051f      	l.li	a0,0xa33248
  a2d9ae:	e96fe0ef          	jal	ra,a2c044 <print_str>
  a2d9b2:	f23fb0ef          	jal	ra,a298d4 <osal_irq_lock>
  a2d9b6:	00a0 58c0 079f      	l.li	a5,0xa058c0
  a2d9bc:	2398                	lbu	a4,0(a5)
  a2d9be:	89aa                	mv	s3,a0
  a2d9c0:	e321                	bnez	a4,a2da00 <panic_deal+0x66>
  a2d9c2:	4705                	li	a4,1
  a2d9c4:	a398                	sb	a4,0(a5)
  a2d9c6:	03210023          	sb	s2,32(sp)
  a2d9ca:	d226                	sw	s1,36(sp)
  a2d9cc:	fe6fc0ef          	jal	ra,a2a1b2 <uapi_systick_get_us>
  a2d9d0:	d42a                	sw	a0,40(sp)
  a2d9d2:	1008                	addi	a0,sp,32
  a2d9d4:	d622                	sw	s0,44(sp)
  a2d9d6:	28d5                	jal	ra,a2daca <set_last_panic>
  a2d9d8:	02000613          	li	a2,32
  a2d9dc:	4581                	li	a1,0
  a2d9de:	1008                	addi	a0,sp,32
  a2d9e0:	c802                	sw	zero,16(sp)
  a2d9e2:	ca02                	sw	zero,20(sp)
  a2d9e4:	cc02                	sw	zero,24(sp)
  a2d9e6:	ce02                	sw	zero,28(sp)
  a2d9e8:	fbdfa0ef          	jal	ra,a289a4 <memset>
  a2d9ec:	da22                	sw	s0,52(sp)
  a2d9ee:	341027f3          	csrr	a5,mepc
  a2d9f2:	dc3e                	sw	a5,56(sp)
  a2d9f4:	878a                	mv	a5,sp
  a2d9f6:	0068                	addi	a0,sp,12
  a2d9f8:	c63e                	sw	a5,12(sp)
  a2d9fa:	2221                	jal	ra,a2db02 <set_exception_info>
  a2d9fc:	1008                	addi	a0,sp,32
  a2d9fe:	2215                	jal	ra,a2db22 <set_exception_stack_frame>
  a2da00:	00a0 58c4 079f      	l.li	a5,0xa058c4
  a2da06:	439c                	lw	a5,0(a5)
  a2da08:	c391                	beqz	a5,a2da0c <panic_deal+0x72>
  a2da0a:	9782                	jalr	a5
  a2da0c:	6589                	lui	a1,0x2
  a2da0e:	058d                	addi	a1,a1,3 # 2003 <ccause+0x1041>
  a2da10:	4509                	li	a0,2
  a2da12:	f73ff0ef          	jal	ra,a2d984 <cpu_utils_reset_chip_with_log>
  a2da16:	854e                	mv	a0,s3
  a2da18:	ec5fb0ef          	jal	ra,a298dc <osal_irq_restore>
  a2da1c:	8454                	popret	{ra,s0-s3},96

00a2da1e <panic>:
  a2da1e:	00a3 3f78 079f      	l.li	a5,0xa33f78
  a2da24:	0007a303          	lw	t1,0(a5) # 400000 <__heap_size+0x3e2a08>
  a2da28:	00030563          	beqz	t1,a2da32 <panic+0x14>
  a2da2c:	ffb08613          	addi	a2,ra,-5
  a2da30:	8302                	jr	t1
  a2da32:	8118                	push	{ra},-32
  a2da34:	4785                	li	a5,1
  a2da36:	00f107a3          	sb	a5,15(sp)
  a2da3a:	00f14783          	lbu	a5,15(sp)
  a2da3e:	9f81                	uxtb	a5
  a2da40:	ffed                	bnez	a5,a2da3a <panic+0x1c>
  a2da42:	8114                	popret	{ra},32

00a2da44 <duplicate_preserve_mem>:
  a2da44:	0fc00693          	li	a3,252
  a2da48:	00a3 3fbc 061f      	l.li	a2,0xa33fbc
  a2da4e:	0fc00593          	li	a1,252
  a2da52:	00a0 58c8 051f      	l.li	a0,0xa058c8
  a2da58:	7190206f          	j	a30970 <memcpy_s>

00a2da5c <set_cpu_utils_system_boot_magic>:
  a2da5c:	00a34737          	lui	a4,0xa34
  a2da60:	fbc70793          	addi	a5,a4,-68 # a33fbc <g_preserve_data_lib>
  a2da64:	0d87d783          	lhu	a5,216(a5)
  a2da68:	6689                	lui	a3,0x2
  a2da6a:	04068613          	addi	a2,a3,64 # 2040 <ccause+0x107e>
  a2da6e:	fbc70713          	addi	a4,a4,-68
  a2da72:	02c78963          	beq	a5,a2,a2daa4 <set_cpu_utils_system_boot_magic+0x48>
  a2da76:	00f66c63          	bltu	a2,a5,a2da8e <set_cpu_utils_system_boot_magic+0x32>
  a2da7a:	c78d                	beqz	a5,a2daa4 <set_cpu_utils_system_boot_magic+0x48>
  a2da7c:	0685                	addi	a3,a3,1
  a2da7e:	02d78363          	beq	a5,a3,a2daa4 <set_cpu_utils_system_boot_magic+0x48>
  a2da82:	dead beaf 079f      	l.li	a5,0xdeadbeaf
  a2da88:	0ef72423          	sw	a5,232(a4)
  a2da8c:	8082                	ret
  a2da8e:	66a1                	lui	a3,0x8
  a2da90:	0685                	addi	a3,a3,1 # 8001 <ccause+0x703f>
  a2da92:	00d78963          	beq	a5,a3,a2daa4 <set_cpu_utils_system_boot_magic+0x48>
  a2da96:	0000 f0f0 069f      	l.li	a3,0xf0f0
  a2da9c:	00d78463          	beq	a5,a3,a2daa4 <set_cpu_utils_system_boot_magic+0x48>
  a2daa0:	6691                	lui	a3,0x4
  a2daa2:	bfe9                	j	a2da7c <set_cpu_utils_system_boot_magic+0x20>
  a2daa4:	dead dead 079f      	l.li	a5,0xdeaddead
  a2daaa:	bff9                	j	a2da88 <set_cpu_utils_system_boot_magic+0x2c>

00a2daac <set_cpu_utils_reset_cause>:
  a2daac:	00a3 3fbc 079f      	l.li	a5,0xa33fbc
  a2dab2:	0c078d23          	sb	zero,218(a5)
  a2dab6:	0ca79c23          	sh	a0,216(a5)
  a2daba:	f8bff06f          	j	a2da44 <duplicate_preserve_mem>

00a2dabe <set_update_reset_cause_on_boot>:
  a2dabe:	00a3 3fbc 079f      	l.li	a5,0xa33fbc
  a2dac4:	0ca78d23          	sb	a0,218(a5)
  a2dac8:	8082                	ret

00a2daca <set_last_panic>:
  a2daca:	8018                	push	{ra},-16
  a2dacc:	862a                	mv	a2,a0
  a2dace:	00a3 3fbc 051f      	l.li	a0,0xa33fbc
  a2dad4:	46c1                	li	a3,16
  a2dad6:	45c1                	li	a1,16
  a2dad8:	0a450513          	addi	a0,a0,164 # 10a4 <ccause+0xe2>
  a2dadc:	695020ef          	jal	ra,a30970 <memcpy_s>
  a2dae0:	8010                	pop	{ra},16
  a2dae2:	f63ff06f          	j	a2da44 <duplicate_preserve_mem>

00a2dae6 <set_exception_info_riscv>:
  a2dae6:	8018                	push	{ra},-16
  a2dae8:	862a                	mv	a2,a0
  a2daea:	09000693          	li	a3,144
  a2daee:	09000593          	li	a1,144
  a2daf2:	00a3 3fbc 051f      	l.li	a0,0xa33fbc
  a2daf8:	679020ef          	jal	ra,a30970 <memcpy_s>
  a2dafc:	8010                	pop	{ra},16
  a2dafe:	f47ff06f          	j	a2da44 <duplicate_preserve_mem>

00a2db02 <set_exception_info>:
  a2db02:	cd19                	beqz	a0,a2db20 <set_exception_info+0x1e>
  a2db04:	8018                	push	{ra},-16
  a2db06:	862a                	mv	a2,a0
  a2db08:	00a3 3fbc 051f      	l.li	a0,0xa33fbc
  a2db0e:	46d1                	li	a3,20
  a2db10:	45d1                	li	a1,20
  a2db12:	09050513          	addi	a0,a0,144
  a2db16:	65b020ef          	jal	ra,a30970 <memcpy_s>
  a2db1a:	8010                	pop	{ra},16
  a2db1c:	f29ff06f          	j	a2da44 <duplicate_preserve_mem>
  a2db20:	8082                	ret

00a2db22 <set_exception_stack_frame>:
  a2db22:	4958                	lw	a4,20(a0)
  a2db24:	00a3 3fbc 079f      	l.li	a5,0xa33fbc
  a2db2a:	08e7a623          	sw	a4,140(a5)
  a2db2e:	4d18                	lw	a4,24(a0)
  a2db30:	cbd8                	sw	a4,20(a5)
  a2db32:	f13ff06f          	j	a2da44 <duplicate_preserve_mem>

00a2db36 <uapi_partition_init>:
  a2db36:	8048                	push	{ra,s0-s2},-16
  a2db38:	ef4ff0ef          	jal	ra,a2d22c <partition_get_addr>
  a2db3c:	4118                	lw	a4,0(a0)
  a2db3e:	4b87 a54b 079f      	l.li	a5,0x4b87a54b
  a2db44:	06f71563          	bne	a4,a5,a2dbae <uapi_partition_init+0x78>
  a2db48:	842a                	mv	s0,a0
  a2db4a:	ee2ff0ef          	jal	ra,a2d22c <partition_get_addr>
  a2db4e:	892a                	mv	s2,a0
  a2db50:	2446                	lhu	s1,12(s0)
  a2db52:	edaff0ef          	jal	ra,a2d22c <partition_get_addr>
  a2db56:	4014                	lw	a3,0(s0)
  a2db58:	00a06737          	lui	a4,0xa06
  a2db5c:	9c470793          	addi	a5,a4,-1596 # a059c4 <g_partition_info>
  a2db60:	c394                	sw	a3,0(a5)
  a2db62:	4054                	lw	a3,4(s0)
  a2db64:	9c470313          	addi	t1,a4,-1596
  a2db68:	c3d4                	sw	a3,4(a5)
  a2db6a:	4414                	lw	a3,8(s0)
  a2db6c:	c794                	sw	a3,8(a5)
  a2db6e:	2474                	lbu	a3,14(s0)
  a2db70:	c7d4                	sw	a3,12(a5)
  a2db72:	1006ef3b          	bltui	a3,16,a2dbae <uapi_partition_init+0x78>
  a2db76:	9c470713          	addi	a4,a4,-1596
  a2db7a:	4781                	li	a5,0
  a2db7c:	94ca                	add	s1,s1,s2
  a2db7e:	4841                	li	a6,16
  a2db80:	06f4861b          	addshf	a2,s1,a5,sll,3
  a2db84:	4214                	lw	a3,0(a2)
  a2db86:	00178593          	addi	a1,a5,1
  a2db8a:	97aa                	add	a5,a5,a0
  a2db8c:	cb14                	sw	a3,16(a4)
  a2db8e:	08b3069b          	addshf	a3,t1,a1,sll,4
  a2db92:	00462883          	lw	a7,4(a2) # 200004 <__heap_size+0x1e2a0c>
  a2db96:	2690                	lbu	a2,8(a3)
  a2db98:	0741                	addi	a4,a4,16
  a2db9a:	1116261b          	orshf	a2,a2,a7,sll,8
  a2db9e:	c690                	sw	a2,8(a3)
  a2dba0:	37fc                	lbu	a5,15(a5)
  a2dba2:	a69c                	sb	a5,8(a3)
  a2dba4:	87ae                	mv	a5,a1
  a2dba6:	fd059de3          	bne	a1,a6,a2db80 <uapi_partition_init+0x4a>
  a2dbaa:	4501                	li	a0,0
  a2dbac:	8044                	popret	{ra,s0-s2},16
  a2dbae:	80003537          	lui	a0,0x80003
  a2dbb2:	bfed                	j	a2dbac <uapi_partition_init+0x76>

00a2dbb4 <uapi_partition_get_info>:
  a2dbb4:	c1bd                	beqz	a1,a2dc1a <uapi_partition_get_info+0x66>
  a2dbb6:	00a06737          	lui	a4,0xa06
  a2dbba:	9c470793          	addi	a5,a4,-1596 # a059c4 <g_partition_info>
  a2dbbe:	47d0                	lw	a2,12(a5)
  a2dbc0:	9c470713          	addi	a4,a4,-1596
  a2dbc4:	4781                	li	a5,0
  a2dbc6:	02c7e863          	bltu	a5,a2,a2dbf6 <uapi_partition_get_info+0x42>
  a2dbca:	8138                	push	{ra,s0-s1},-32
  a2dbcc:	84aa                	mv	s1,a0
  a2dbce:	0068                	addi	a0,sp,12
  a2dbd0:	842e                	mv	s0,a1
  a2dbd2:	c602                	sw	zero,12(sp)
  a2dbd4:	e60ff0ef          	jal	ra,a2d234 <partition_get_path_map>
  a2dbd8:	c919                	beqz	a0,a2dbee <uapi_partition_get_info+0x3a>
  a2dbda:	47b2                	lw	a5,12(sp)
  a2dbdc:	cb89                	beqz	a5,a2dbee <uapi_partition_get_info+0x3a>
  a2dbde:	4701                	li	a4,0
  a2dbe0:	86be                	mv	a3,a5
  a2dbe2:	4290                	lw	a2,0(a3)
  a2dbe4:	07a1                	addi	a5,a5,8
  a2dbe6:	02c49163          	bne	s1,a2,a2dc08 <uapi_partition_get_info+0x54>
  a2dbea:	42dc                	lw	a5,4(a3)
  a2dbec:	e395                	bnez	a5,a2dc10 <uapi_partition_get_info+0x5c>
  a2dbee:	8000 3003 051f      	l.li	a0,0x80003003
  a2dbf4:	8134                	popret	{ra,s0-s1},32
  a2dbf6:	0785                	addi	a5,a5,1
  a2dbf8:	08f7069b          	addshf	a3,a4,a5,sll,4
  a2dbfc:	0086c803          	lbu	a6,8(a3) # 4008 <ccause+0x3046>
  a2dc00:	02a80163          	beq	a6,a0,a2dc22 <uapi_partition_get_info+0x6e>
  a2dc04:	9f81                	uxtb	a5
  a2dc06:	b7c1                	j	a2dbc6 <uapi_partition_get_info+0x12>
  a2dc08:	0705                	addi	a4,a4,1
  a2dc0a:	fce51be3          	bne	a0,a4,a2dbe0 <uapi_partition_get_info+0x2c>
  a2dc0e:	b7c5                	j	a2dbee <uapi_partition_get_info+0x3a>
  a2dc10:	4705                	li	a4,1
  a2dc12:	a018                	sb	a4,0(s0)
  a2dc14:	c05c                	sw	a5,4(s0)
  a2dc16:	4501                	li	a0,0
  a2dc18:	bff1                	j	a2dbf4 <uapi_partition_get_info+0x40>
  a2dc1a:	8000 3002 051f      	l.li	a0,0x80003002
  a2dc20:	8082                	ret
  a2dc22:	00058023          	sb	zero,0(a1)
  a2dc26:	429c                	lw	a5,0(a3)
  a2dc28:	4501                	li	a0,0
  a2dc2a:	c1dc                	sw	a5,4(a1)
  a2dc2c:	469c                	lw	a5,8(a3)
  a2dc2e:	83a1                	srli	a5,a5,0x8
  a2dc30:	c59c                	sw	a5,8(a1)
  a2dc32:	8082                	ret

00a2dc34 <upg_malloc>:
  a2dc34:	8028                	push	{ra,s0},-16
  a2dc36:	842a                	mv	s0,a0
  a2dc38:	2b8d                	jal	ra,a2e1aa <upg_get_func_list>
  a2dc3a:	411c                	lw	a5,0(a0)
  a2dc3c:	c799                	beqz	a5,a2dc4a <upg_malloc+0x16>
  a2dc3e:	23b5                	jal	ra,a2e1aa <upg_get_func_list>
  a2dc40:	00052303          	lw	t1,0(a0) # 80003000 <_gp_+0x7f5ce844>
  a2dc44:	8522                	mv	a0,s0
  a2dc46:	8020                	pop	{ra,s0},16
  a2dc48:	8302                	jr	t1
  a2dc4a:	4501                	li	a0,0
  a2dc4c:	8024                	popret	{ra,s0},16

00a2dc4e <upg_free>:
  a2dc4e:	c51d                	beqz	a0,a2dc7c <upg_free+0x2e>
  a2dc50:	8038                	push	{ra,s0-s1},-16
  a2dc52:	842a                	mv	s0,a0
  a2dc54:	f7aff0ef          	jal	ra,a2d3ce <upg_get_flash_base_addr>
  a2dc58:	84aa                	mv	s1,a0
  a2dc5a:	f7aff0ef          	jal	ra,a2d3d4 <upg_get_flash_size>
  a2dc5e:	00946563          	bltu	s0,s1,a2dc68 <upg_free+0x1a>
  a2dc62:	94aa                	add	s1,s1,a0
  a2dc64:	00946b63          	bltu	s0,s1,a2dc7a <upg_free+0x2c>
  a2dc68:	2389                	jal	ra,a2e1aa <upg_get_func_list>
  a2dc6a:	415c                	lw	a5,4(a0)
  a2dc6c:	c799                	beqz	a5,a2dc7a <upg_free+0x2c>
  a2dc6e:	2b35                	jal	ra,a2e1aa <upg_get_func_list>
  a2dc70:	00452303          	lw	t1,4(a0)
  a2dc74:	8522                	mv	a0,s0
  a2dc76:	8030                	pop	{ra,s0-s1},16
  a2dc78:	8302                	jr	t1
  a2dc7a:	8034                	popret	{ra,s0-s1},16
  a2dc7c:	8082                	ret

00a2dc7e <upg_get_ctx>:
  a2dc7e:	00a0 5ad4 051f      	l.li	a0,0xa05ad4
  a2dc84:	8082                	ret

00a2dc86 <upg_alloc_and_get_upgrade_flag>:
  a2dc86:	8138                	push	{ra,s0-s1},-32
  a2dc88:	84aa                	mv	s1,a0
  a2dc8a:	0068                	addi	a0,sp,12
  a2dc8c:	c602                	sw	zero,12(sp)
  a2dc8e:	eb2ff0ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a2dc92:	c909                	beqz	a0,a2dca4 <upg_alloc_and_get_upgrade_flag+0x1e>
  a2dc94:	842a                	mv	s0,a0
  a2dc96:	00a3 3270 051f      	l.li	a0,0xa33270
  a2dc9c:	ba8fe0ef          	jal	ra,a2c044 <print_str>
  a2dca0:	8522                	mv	a0,s0
  a2dca2:	8134                	popret	{ra,s0-s1},32
  a2dca4:	06c00513          	li	a0,108
  a2dca8:	f8dff0ef          	jal	ra,a2dc34 <upg_malloc>
  a2dcac:	c088                	sw	a0,0(s1)
  a2dcae:	862a                	mv	a2,a0
  a2dcb0:	e911                	bnez	a0,a2dcc4 <upg_alloc_and_get_upgrade_flag+0x3e>
  a2dcb2:	00a3 32a4 051f      	l.li	a0,0xa332a4
  a2dcb8:	80000437          	lui	s0,0x80000
  a2dcbc:	b88fe0ef          	jal	ra,a2c044 <print_str>
  a2dcc0:	0415                	addi	s0,s0,5 # 80000005 <_gp_+0x7f5cb849>
  a2dcc2:	bff9                	j	a2dca0 <upg_alloc_and_get_upgrade_flag+0x1a>
  a2dcc4:	4532                	lw	a0,12(sp)
  a2dcc6:	06c00593          	li	a1,108
  a2dcca:	f10ff0ef          	jal	ra,a2d3da <upg_flash_read>
  a2dcce:	842a                	mv	s0,a0
  a2dcd0:	d961                	beqz	a0,a2dca0 <upg_alloc_and_get_upgrade_flag+0x1a>
  a2dcd2:	00a3 32dc 051f      	l.li	a0,0xa332dc
  a2dcd8:	b6cfe0ef          	jal	ra,a2c044 <print_str>
  a2dcdc:	4088                	lw	a0,0(s1)
  a2dcde:	f71ff0ef          	jal	ra,a2dc4e <upg_free>
  a2dce2:	0004a023          	sw	zero,0(s1) # fffff000 <_gp_+0xff5ca844>
  a2dce6:	bf6d                	j	a2dca0 <upg_alloc_and_get_upgrade_flag+0x1a>

00a2dce8 <upg_get_image_info>:
  a2dce8:	cd9d                	beqz	a1,a2dd26 <upg_get_image_info+0x3e>
  a2dcea:	8228                	push	{ra,s0},-48
  a2dcec:	842a                	mv	s0,a0
  a2dcee:	0868                	addi	a0,sp,28
  a2dcf0:	c62e                	sw	a1,12(sp)
  a2dcf2:	ce02                	sw	zero,28(sp)
  a2dcf4:	e1aff0ef          	jal	ra,a2d30e <upg_get_ids_map>
  a2dcf8:	45b2                	lw	a1,12(sp)
  a2dcfa:	e509                	bnez	a0,a2dd04 <upg_get_image_info+0x1c>
  a2dcfc:	8000 3002 051f      	l.li	a0,0x80003002
  a2dd02:	8224                	popret	{ra,s0},48
  a2dd04:	47f2                	lw	a5,28(sp)
  a2dd06:	dbfd                	beqz	a5,a2dcfc <upg_get_image_info+0x14>
  a2dd08:	4701                	li	a4,0
  a2dd0a:	86be                	mv	a3,a5
  a2dd0c:	07a1                	addi	a5,a5,8
  a2dd0e:	ff87a603          	lw	a2,-8(a5)
  a2dd12:	00c41663          	bne	s0,a2,a2dd1e <upg_get_image_info+0x36>
  a2dd16:	22c8                	lbu	a0,4(a3)
  a2dd18:	e9dff0ef          	jal	ra,a2dbb4 <uapi_partition_get_info>
  a2dd1c:	b7dd                	j	a2dd02 <upg_get_image_info+0x1a>
  a2dd1e:	0705                	addi	a4,a4,1
  a2dd20:	fee515e3          	bne	a0,a4,a2dd0a <upg_get_image_info+0x22>
  a2dd24:	bfe1                	j	a2dcfc <upg_get_image_info+0x14>
  a2dd26:	8000 3002 051f      	l.li	a0,0x80003002
  a2dd2c:	8082                	ret

00a2dd2e <upg_read_fota_pkg_data>:
  a2dd2e:	8158                	push	{ra,s0-s3},-48
  a2dd30:	842a                	mv	s0,a0
  a2dd32:	89ae                	mv	s3,a1
  a2dd34:	0028                	addi	a0,sp,8
  a2dd36:	006c                	addi	a1,sp,12
  a2dd38:	8932                	mv	s2,a2
  a2dd3a:	c402                	sw	zero,8(sp)
  a2dd3c:	c602                	sw	zero,12(sp)
  a2dd3e:	e44ff0ef          	jal	ra,a2d382 <upg_get_fota_partiton_area_addr>
  a2dd42:	e51d                	bnez	a0,a2dd70 <upg_read_fota_pkg_data+0x42>
  a2dd44:	47b2                	lw	a5,12(sp)
  a2dd46:	02f47663          	bgeu	s0,a5,a2dd72 <upg_read_fota_pkg_data+0x44>
  a2dd4a:	00092483          	lw	s1,0(s2) # fffff000 <_gp_+0xff5ca844>
  a2dd4e:	c095                	beqz	s1,a2dd72 <upg_read_fota_pkg_data+0x44>
  a2dd50:	00848733          	add	a4,s1,s0
  a2dd54:	00e7f463          	bgeu	a5,a4,a2dd5c <upg_read_fota_pkg_data+0x2e>
  a2dd58:	408784b3          	sub	s1,a5,s0
  a2dd5c:	4522                	lw	a0,8(sp)
  a2dd5e:	864e                	mv	a2,s3
  a2dd60:	85a6                	mv	a1,s1
  a2dd62:	9522                	add	a0,a0,s0
  a2dd64:	c42a                	sw	a0,8(sp)
  a2dd66:	e74ff0ef          	jal	ra,a2d3da <upg_flash_read>
  a2dd6a:	e119                	bnez	a0,a2dd70 <upg_read_fota_pkg_data+0x42>
  a2dd6c:	00992023          	sw	s1,0(s2)
  a2dd70:	8154                	popret	{ra,s0-s3},48
  a2dd72:	8000 3042 051f      	l.li	a0,0x80003042
  a2dd78:	bfe5                	j	a2dd70 <upg_read_fota_pkg_data+0x42>

00a2dd7a <uapi_upg_get_storage_size>:
  a2dd7a:	8128                	push	{ra,s0},-32
  a2dd7c:	00a0 5ad4 079f      	l.li	a5,0xa05ad4
  a2dd82:	239c                	lbu	a5,0(a5)
  a2dd84:	4401                	li	s0,0
  a2dd86:	cb99                	beqz	a5,a2dd9c <uapi_upg_get_storage_size+0x22>
  a2dd88:	006c                	addi	a1,sp,12
  a2dd8a:	0028                	addi	a0,sp,8
  a2dd8c:	c402                	sw	zero,8(sp)
  a2dd8e:	c602                	sw	zero,12(sp)
  a2dd90:	df2ff0ef          	jal	ra,a2d382 <upg_get_fota_partiton_area_addr>
  a2dd94:	e501                	bnez	a0,a2dd9c <uapi_upg_get_storage_size+0x22>
  a2dd96:	47b2                	lw	a5,12(sp)
  a2dd98:	7475                	lui	s0,0xffffd
  a2dd9a:	943e                	add	s0,s0,a5
  a2dd9c:	8522                	mv	a0,s0
  a2dd9e:	8124                	popret	{ra,s0},32

00a2dda0 <upg_get_partition_info>:
  a2dda0:	8138                	push	{ra,s0-s1},-32
  a2dda2:	c202                	sw	zero,4(sp)
  a2dda4:	c402                	sw	zero,8(sp)
  a2dda6:	c602                	sw	zero,12(sp)
  a2dda8:	4b87 a52d 079f      	l.li	a5,0x4b87a52d
  a2ddae:	84ae                	mv	s1,a1
  a2ddb0:	8432                	mv	s0,a2
  a2ddb2:	02f51163          	bne	a0,a5,a2ddd4 <upg_get_partition_info+0x34>
  a2ddb6:	002007b7          	lui	a5,0x200
  a2ddba:	c43e                	sw	a5,8(sp)
  a2ddbc:	78000793          	li	a5,1920
  a2ddc0:	c63e                	sw	a5,12(sp)
  a2ddc2:	00414783          	lbu	a5,4(sp)
  a2ddc6:	ef81                	bnez	a5,a2ddde <upg_get_partition_info+0x3e>
  a2ddc8:	47a2                	lw	a5,8(sp)
  a2ddca:	c09c                	sw	a5,0(s1)
  a2ddcc:	47b2                	lw	a5,12(sp)
  a2ddce:	c01c                	sw	a5,0(s0)
  a2ddd0:	4501                	li	a0,0
  a2ddd2:	a029                	j	a2dddc <upg_get_partition_info+0x3c>
  a2ddd4:	004c                	addi	a1,sp,4
  a2ddd6:	f13ff0ef          	jal	ra,a2dce8 <upg_get_image_info>
  a2ddda:	d565                	beqz	a0,a2ddc2 <upg_get_partition_info+0x22>
  a2dddc:	8134                	popret	{ra,s0-s1},32
  a2ddde:	0004a023          	sw	zero,0(s1)
  a2dde2:	f99ff0ef          	jal	ra,a2dd7a <uapi_upg_get_storage_size>
  a2dde6:	c008                	sw	a0,0(s0)
  a2dde8:	b7e5                	j	a2ddd0 <upg_get_partition_info+0x30>

00a2ddea <upg_get_package_header>:
  a2ddea:	8138                	push	{ra,s0-s1},-32
  a2ddec:	84aa                	mv	s1,a0
  a2ddee:	006c                	addi	a1,sp,12
  a2ddf0:	0028                	addi	a0,sp,8
  a2ddf2:	c402                	sw	zero,8(sp)
  a2ddf4:	c602                	sw	zero,12(sp)
  a2ddf6:	d8cff0ef          	jal	ra,a2d382 <upg_get_fota_partiton_area_addr>
  a2ddfa:	842a                	mv	s0,a0
  a2ddfc:	e511                	bnez	a0,a2de08 <upg_get_package_header+0x1e>
  a2ddfe:	dd0ff0ef          	jal	ra,a2d3ce <upg_get_flash_base_addr>
  a2de02:	47a2                	lw	a5,8(sp)
  a2de04:	953e                	add	a0,a0,a5
  a2de06:	c088                	sw	a0,0(s1)
  a2de08:	8522                	mv	a0,s0
  a2de0a:	8134                	popret	{ra,s0-s1},32

00a2de0c <upg_get_pkg_image_hash_table>:
  a2de0c:	8148                	push	{ra,s0-s2},-32
  a2de0e:	12052403          	lw	s0,288(a0)
  a2de12:	892e                	mv	s2,a1
  a2de14:	e019                	bnez	s0,a2de1a <upg_get_pkg_image_hash_table+0xe>
  a2de16:	20000413          	li	s0,512
  a2de1a:	006c                	addi	a1,sp,12
  a2de1c:	0028                	addi	a0,sp,8
  a2de1e:	c402                	sw	zero,8(sp)
  a2de20:	c602                	sw	zero,12(sp)
  a2de22:	d60ff0ef          	jal	ra,a2d382 <upg_get_fota_partiton_area_addr>
  a2de26:	84aa                	mv	s1,a0
  a2de28:	e911                	bnez	a0,a2de3c <upg_get_pkg_image_hash_table+0x30>
  a2de2a:	47a2                	lw	a5,8(sp)
  a2de2c:	97a2                	add	a5,a5,s0
  a2de2e:	c43e                	sw	a5,8(sp)
  a2de30:	d9eff0ef          	jal	ra,a2d3ce <upg_get_flash_base_addr>
  a2de34:	47a2                	lw	a5,8(sp)
  a2de36:	953e                	add	a0,a0,a5
  a2de38:	00a92023          	sw	a0,0(s2)
  a2de3c:	8526                	mv	a0,s1
  a2de3e:	8144                	popret	{ra,s0-s2},32

00a2de40 <upg_get_pkg_image_header>:
  a2de40:	8148                	push	{ra,s0-s2},-32
  a2de42:	892a                	mv	s2,a0
  a2de44:	84ae                	mv	s1,a1
  a2de46:	0028                	addi	a0,sp,8
  a2de48:	006c                	addi	a1,sp,12
  a2de4a:	c402                	sw	zero,8(sp)
  a2de4c:	c602                	sw	zero,12(sp)
  a2de4e:	d34ff0ef          	jal	ra,a2d382 <upg_get_fota_partiton_area_addr>
  a2de52:	842a                	mv	s0,a0
  a2de54:	e919                	bnez	a0,a2de6a <upg_get_pkg_image_header+0x2a>
  a2de56:	47a2                	lw	a5,8(sp)
  a2de58:	00492703          	lw	a4,4(s2)
  a2de5c:	97ba                	add	a5,a5,a4
  a2de5e:	c43e                	sw	a5,8(sp)
  a2de60:	d6eff0ef          	jal	ra,a2d3ce <upg_get_flash_base_addr>
  a2de64:	47a2                	lw	a5,8(sp)
  a2de66:	953e                	add	a0,a0,a5
  a2de68:	c088                	sw	a0,0(s1)
  a2de6a:	8522                	mv	a0,s0
  a2de6c:	8144                	popret	{ra,s0-s2},32

00a2de6e <upg_get_pkg_image_data>:
  a2de6e:	8178                	push	{ra,s0-s5},-48
  a2de70:	4544                	lw	s1,12(a0)
  a2de72:	8aaa                	mv	s5,a0
  a2de74:	842e                	mv	s0,a1
  a2de76:	0028                	addi	a0,sp,8
  a2de78:	006c                	addi	a1,sp,12
  a2de7a:	89b2                	mv	s3,a2
  a2de7c:	8a36                	mv	s4,a3
  a2de7e:	c402                	sw	zero,8(sp)
  a2de80:	c602                	sw	zero,12(sp)
  a2de82:	d00ff0ef          	jal	ra,a2d382 <upg_get_fota_partiton_area_addr>
  a2de86:	892a                	mv	s2,a0
  a2de88:	e121                	bnez	a0,a2dec8 <upg_get_pkg_image_data+0x5a>
  a2de8a:	00f48793          	addi	a5,s1,15
  a2de8e:	9bc1                	andi	a5,a5,-16
  a2de90:	02f47e63          	bgeu	s0,a5,a2decc <upg_get_pkg_image_data+0x5e>
  a2de94:	020a0c63          	beqz	s4,a2decc <upg_get_pkg_image_data+0x5e>
  a2de98:	02098a63          	beqz	s3,a2decc <upg_get_pkg_image_data+0x5e>
  a2de9c:	0009a483          	lw	s1,0(s3) # fffff000 <_gp_+0xff5ca844>
  a2dea0:	c495                	beqz	s1,a2decc <upg_get_pkg_image_data+0x5e>
  a2dea2:	00848733          	add	a4,s1,s0
  a2dea6:	00e7f463          	bgeu	a5,a4,a2deae <upg_get_pkg_image_data+0x40>
  a2deaa:	408784b3          	sub	s1,a5,s0
  a2deae:	47a2                	lw	a5,8(sp)
  a2deb0:	008aa503          	lw	a0,8(s5)
  a2deb4:	953e                	add	a0,a0,a5
  a2deb6:	942a                	add	s0,s0,a0
  a2deb8:	c42a                	sw	a0,8(sp)
  a2deba:	d14ff0ef          	jal	ra,a2d3ce <upg_get_flash_base_addr>
  a2debe:	9522                	add	a0,a0,s0
  a2dec0:	00aa2023          	sw	a0,0(s4)
  a2dec4:	0099a023          	sw	s1,0(s3)
  a2dec8:	854a                	mv	a0,s2
  a2deca:	8174                	popret	{ra,s0-s5},48
  a2decc:	8000 3042 091f      	l.li	s2,0x80003042
  a2ded2:	bfdd                	j	a2dec8 <upg_get_pkg_image_data+0x5a>

00a2ded4 <upg_copy_pkg_image_data>:
  a2ded4:	455c                	lw	a5,12(a0)
  a2ded6:	07bd                	addi	a5,a5,15 # 20000f <__heap_size+0x1e2a17>
  a2ded8:	9bc1                	andi	a5,a5,-16
  a2deda:	02f5fb63          	bgeu	a1,a5,a2df10 <upg_copy_pkg_image_data+0x3c>
  a2dede:	ca8d                	beqz	a3,a2df10 <upg_copy_pkg_image_data+0x3c>
  a2dee0:	ca05                	beqz	a2,a2df10 <upg_copy_pkg_image_data+0x3c>
  a2dee2:	00062803          	lw	a6,0(a2)
  a2dee6:	02080563          	beqz	a6,a2df10 <upg_copy_pkg_image_data+0x3c>
  a2deea:	8128                	push	{ra,s0},-32
  a2deec:	872e                	mv	a4,a1
  a2deee:	95c2                	add	a1,a1,a6
  a2def0:	00b7f463          	bgeu	a5,a1,a2def8 <upg_copy_pkg_image_data+0x24>
  a2def4:	40e78833          	sub	a6,a5,a4
  a2def8:	4508                	lw	a0,8(a0)
  a2defa:	8432                	mv	s0,a2
  a2defc:	85b6                	mv	a1,a3
  a2defe:	0070                	addi	a2,sp,12
  a2df00:	953a                	add	a0,a0,a4
  a2df02:	c642                	sw	a6,12(sp)
  a2df04:	e2bff0ef          	jal	ra,a2dd2e <upg_read_fota_pkg_data>
  a2df08:	e119                	bnez	a0,a2df0e <upg_copy_pkg_image_data+0x3a>
  a2df0a:	47b2                	lw	a5,12(sp)
  a2df0c:	c01c                	sw	a5,0(s0)
  a2df0e:	8124                	popret	{ra,s0},32
  a2df10:	8000 3042 051f      	l.li	a0,0x80003042
  a2df16:	8082                	ret

00a2df18 <upg_set_firmware_update_status>:
  a2df18:	8168                	push	{ra,s0-s4},-48
  a2df1a:	cb9e 063c 079f      	l.li	a5,0xcb9e063c
  a2df20:	02f59163          	bne	a1,a5,a2df42 <upg_set_firmware_update_status+0x2a>
  a2df24:	04c50513          	addi	a0,a0,76
  a2df28:	211c                	lbu	a5,0(a0)
  a2df2a:	efb5                	bnez	a5,a2dfa6 <upg_set_firmware_update_status+0x8e>
  a2df2c:	311c                	lbu	a5,1(a0)
  a2df2e:	00150913          	addi	s2,a0,1
  a2df32:	ebe1                	bnez	a5,a2e002 <upg_set_firmware_update_status+0xea>
  a2df34:	213c                	lbu	a5,2(a0)
  a2df36:	00250913          	addi	s2,a0,2
  a2df3a:	e7f1                	bnez	a5,a2e006 <upg_set_firmware_update_status+0xee>
  a2df3c:	4401                	li	s0,0
  a2df3e:	8522                	mv	a0,s0
  a2df40:	8164                	popret	{ra,s0-s4},48
  a2df42:	02b5555b          	muliadd	a0,a0,a1,3
  a2df46:	0541                	addi	a0,a0,16
  a2df48:	b7c5                	j	a2df28 <upg_set_firmware_update_status+0x10>
  a2df4a:	0441                	addi	s0,s0,16 # ffffd010 <_gp_+0xff5c8854>
  a2df4c:	029455db          	muliadd	a1,s0,s1,3
  a2df50:	00b98433          	add	s0,s3,a1
  a2df54:	a8a5                	j	a2dfcc <upg_set_firmware_update_status+0xb4>
  a2df56:	010a14bb          	bnei	s4,1,a2df68 <upg_set_firmware_update_status+0x50>
  a2df5a:	00094783          	lbu	a5,0(s2)
  a2df5e:	0ff797bb          	bnei	a5,15,a2df3c <upg_set_firmware_update_status+0x24>
  a2df62:	00090023          	sb	zero,0(s2)
  a2df66:	a8b5                	j	a2dfe2 <upg_set_firmware_update_status+0xca>
  a2df68:	02fa153b          	bnei	s4,2,a2df3c <upg_set_firmware_update_status+0x24>
  a2df6c:	448d                	li	s1,3
  a2df6e:	413484b3          	sub	s1,s1,s3
  a2df72:	86a6                	mv	a3,s1
  a2df74:	4601                	li	a2,0
  a2df76:	85a6                	mv	a1,s1
  a2df78:	854a                	mv	a0,s2
  a2df7a:	273020ef          	jal	ra,a309ec <memset_s>
  a2df7e:	a09d                	j	a2dfe4 <upg_set_firmware_update_status+0xcc>
  a2df80:	02e99f3b          	bnei	s3,2,a2df3c <upg_set_firmware_update_status+0x24>
  a2df84:	00094783          	lbu	a5,0(s2)
  a2df88:	fbd5                	bnez	a5,a2df3c <upg_set_firmware_update_status+0x24>
  a2df8a:	00a3 3390 051f      	l.li	a0,0xa33390
  a2df90:	8b4fe0ef          	jal	ra,a2c044 <print_str>
  a2df94:	00a0 5ad4 079f      	l.li	a5,0xa05ad4
  a2df9a:	4735                	li	a4,13
  a2df9c:	b3d8                	sb	a4,5(a5)
  a2df9e:	8000 3067 041f      	l.li	s0,0x80003067
  a2dfa4:	bf69                	j	a2df3e <upg_set_firmware_update_status+0x26>
  a2dfa6:	892a                	mv	s2,a0
  a2dfa8:	4981                	li	s3,0
  a2dfaa:	0068                	addi	a0,sp,12
  a2dfac:	8a32                	mv	s4,a2
  a2dfae:	84ae                	mv	s1,a1
  a2dfb0:	c602                	sw	zero,12(sp)
  a2dfb2:	b8eff0ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a2dfb6:	842a                	mv	s0,a0
  a2dfb8:	f159                	bnez	a0,a2df3e <upg_set_firmware_update_status+0x26>
  a2dfba:	cb9e 063c 079f      	l.li	a5,0xcb9e063c
  a2dfc0:	4432                	lw	s0,12(sp)
  a2dfc2:	f8f494e3          	bne	s1,a5,a2df4a <upg_set_firmware_update_status+0x32>
  a2dfc6:	04c40413          	addi	s0,s0,76
  a2dfca:	944e                	add	s0,s0,s3
  a2dfcc:	f80a15e3          	bnez	s4,a2df56 <upg_set_firmware_update_status+0x3e>
  a2dfd0:	00094703          	lbu	a4,0(s2)
  a2dfd4:	0ff00793          	li	a5,255
  a2dfd8:	f6f712e3          	bne	a4,a5,a2df3c <upg_set_firmware_update_status+0x24>
  a2dfdc:	47bd                	li	a5,15
  a2dfde:	00f90023          	sb	a5,0(s2)
  a2dfe2:	4485                	li	s1,1
  a2dfe4:	8522                	mv	a0,s0
  a2dfe6:	4681                	li	a3,0
  a2dfe8:	864a                	mv	a2,s2
  a2dfea:	85a6                	mv	a1,s1
  a2dfec:	cb2ff0ef          	jal	ra,a2d49e <upg_flash_write>
  a2dff0:	842a                	mv	s0,a0
  a2dff2:	d559                	beqz	a0,a2df80 <upg_set_firmware_update_status+0x68>
  a2dff4:	85aa                	mv	a1,a0
  a2dff6:	00a3 3358 051f      	l.li	a0,0xa33358
  a2dffc:	848fe0ef          	jal	ra,a2c044 <print_str>
  a2e000:	bf3d                	j	a2df3e <upg_set_firmware_update_status+0x26>
  a2e002:	4985                	li	s3,1
  a2e004:	b75d                	j	a2dfaa <upg_set_firmware_update_status+0x92>
  a2e006:	4989                	li	s3,2
  a2e008:	b74d                	j	a2dfaa <upg_set_firmware_update_status+0x92>

00a2e00a <upg_get_image_update_status>:
  a2e00a:	8128                	push	{ra,s0},-32
  a2e00c:	00a3 1c18 079f      	l.li	a5,0xa31c18
  a2e012:	239a                	lhu	a4,0(a5)
  a2e014:	23bc                	lbu	a5,2(a5)
  a2e016:	00011423          	sh	zero,8(sp)
  a2e01a:	00010523          	sb	zero,10(sp)
  a2e01e:	00f10723          	sb	a5,14(sp)
  a2e022:	00e11623          	sh	a4,12(sp)
  a2e026:	cb9e 063c 079f      	l.li	a5,0xcb9e063c
  a2e02c:	04c50413          	addi	s0,a0,76
  a2e030:	00f60563          	beq	a2,a5,a2e03a <upg_get_image_update_status+0x30>
  a2e034:	02b5545b          	muliadd	s0,a0,a1,3
  a2e038:	0441                	addi	s0,s0,16
  a2e03a:	460d                	li	a2,3
  a2e03c:	002c                	addi	a1,sp,8
  a2e03e:	8522                	mv	a0,s0
  a2e040:	caffa0ef          	jal	ra,a28cee <memcmp>
  a2e044:	478d                	li	a5,3
  a2e046:	cd0d                	beqz	a0,a2e080 <upg_get_image_update_status+0x76>
  a2e048:	460d                	li	a2,3
  a2e04a:	006c                	addi	a1,sp,12
  a2e04c:	8522                	mv	a0,s0
  a2e04e:	ca1fa0ef          	jal	ra,a28cee <memcmp>
  a2e052:	4781                	li	a5,0
  a2e054:	c515                	beqz	a0,a2e080 <upg_get_image_update_status+0x76>
  a2e056:	201c                	lbu	a5,0(s0)
  a2e058:	4501                	li	a0,0
  a2e05a:	eb81                	bnez	a5,a2e06a <upg_get_image_update_status+0x60>
  a2e05c:	301c                	lbu	a5,1(s0)
  a2e05e:	4505                	li	a0,1
  a2e060:	e789                	bnez	a5,a2e06a <upg_get_image_update_status+0x60>
  a2e062:	2028                	lbu	a0,2(s0)
  a2e064:	00153513          	seqz	a0,a0
  a2e068:	0509                	addi	a0,a0,2
  a2e06a:	942a                	add	s0,s0,a0
  a2e06c:	2018                	lbu	a4,0(s0)
  a2e06e:	4785                	li	a5,1
  a2e070:	0f07043b          	beqi	a4,15,a2e080 <upg_get_image_update_status+0x76>
  a2e074:	0ff00693          	li	a3,255
  a2e078:	4789                	li	a5,2
  a2e07a:	00d70363          	beq	a4,a3,a2e080 <upg_get_image_update_status+0x76>
  a2e07e:	4791                	li	a5,4
  a2e080:	853e                	mv	a0,a5
  a2e082:	8124                	popret	{ra,s0},32

00a2e084 <upg_flash_erase_metadata_pages>:
  a2e084:	4501                	li	a0,0
  a2e086:	8082                	ret

00a2e088 <upg_set_temporary_result>:
  a2e088:	00a0 5ad4 079f      	l.li	a5,0xa05ad4
  a2e08e:	b3c8                	sb	a0,5(a5)
  a2e090:	8082                	ret

00a2e092 <upg_set_update_result>:
  a2e092:	8118                	push	{ra},-32
  a2e094:	c62a                	sw	a0,12(sp)
  a2e096:	0028                	addi	a0,sp,8
  a2e098:	c402                	sw	zero,8(sp)
  a2e09a:	aa6ff0ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a2e09e:	e909                	bnez	a0,a2e0b0 <upg_set_update_result+0x1e>
  a2e0a0:	4522                	lw	a0,8(sp)
  a2e0a2:	4681                	li	a3,0
  a2e0a4:	0070                	addi	a2,sp,12
  a2e0a6:	4591                	li	a1,4
  a2e0a8:	05050513          	addi	a0,a0,80
  a2e0ac:	bf2ff0ef          	jal	ra,a2d49e <upg_flash_write>
  a2e0b0:	8114                	popret	{ra},32

00a2e0b2 <upg_check_image_update_complete>:
  a2e0b2:	8158                	push	{ra,s0-s3},-48
  a2e0b4:	842a                	mv	s0,a0
  a2e0b6:	89ae                	mv	s3,a1
  a2e0b8:	00011623          	sh	zero,12(sp)
  a2e0bc:	00010723          	sb	zero,14(sp)
  a2e0c0:	01050913          	addi	s2,a0,16
  a2e0c4:	4481                	li	s1,0
  a2e0c6:	445c                	lw	a5,12(s0)
  a2e0c8:	00f4ea63          	bltu	s1,a5,a2e0dc <upg_check_image_update_complete+0x2a>
  a2e0cc:	4505                	li	a0,1
  a2e0ce:	01378f63          	beq	a5,s3,a2e0ec <upg_check_image_update_complete+0x3a>
  a2e0d2:	04c44783          	lbu	a5,76(s0)
  a2e0d6:	0f07953b          	bnei	a5,15,a2e0ea <upg_check_image_update_complete+0x38>
  a2e0da:	a809                	j	a2e0ec <upg_check_image_update_complete+0x3a>
  a2e0dc:	854a                	mv	a0,s2
  a2e0de:	460d                	li	a2,3
  a2e0e0:	006c                	addi	a1,sp,12
  a2e0e2:	c0dfa0ef          	jal	ra,a28cee <memcmp>
  a2e0e6:	090d                	addi	s2,s2,3
  a2e0e8:	c119                	beqz	a0,a2e0ee <upg_check_image_update_complete+0x3c>
  a2e0ea:	4501                	li	a0,0
  a2e0ec:	8154                	popret	{ra,s0-s3},48
  a2e0ee:	0485                	addi	s1,s1,1
  a2e0f0:	bfd9                	j	a2e0c6 <upg_check_image_update_complete+0x14>

00a2e0f2 <upg_set_complete_flag>:
  a2e0f2:	8848                	push	{ra,s0-s2},-144
  a2e0f4:	84aa                	mv	s1,a0
  a2e0f6:	0068                	addi	a0,sp,12
  a2e0f8:	842e                	mv	s0,a1
  a2e0fa:	8932                	mv	s2,a2
  a2e0fc:	c602                	sw	zero,12(sp)
  a2e0fe:	a42ff0ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a2e102:	ed09                	bnez	a0,a2e11c <upg_set_complete_flag+0x2a>
  a2e104:	4532                	lw	a0,12(sp)
  a2e106:	0850                	addi	a2,sp,20
  a2e108:	06c00593          	li	a1,108
  a2e10c:	aceff0ef          	jal	ra,a2d3da <upg_flash_read>
  a2e110:	c519                	beqz	a0,a2e11e <upg_set_complete_flag+0x2c>
  a2e112:	00a3 3314 051f      	l.li	a0,0xa33314
  a2e118:	f2dfd0ef          	jal	ra,a2c044 <print_str>
  a2e11c:	8844                	popret	{ra,s0-s2},144
  a2e11e:	00090c63          	beqz	s2,a2e136 <upg_set_complete_flag+0x44>
  a2e122:	57e6                	lw	a5,120(sp)
  a2e124:	cb89                	beqz	a5,a2e136 <upg_set_complete_flag+0x44>
  a2e126:	00a0 5ad4 079f      	l.li	a5,0xa05ad4
  a2e12c:	33c8                	lbu	a0,5(a5)
  a2e12e:	f65ff0ef          	jal	ra,a2e092 <upg_set_update_result>
  a2e132:	4581                	li	a1,0
  a2e134:	a821                	j	a2e14c <upg_set_complete_flag+0x5a>
  a2e136:	85a6                	mv	a1,s1
  a2e138:	0848                	addi	a0,sp,20
  a2e13a:	f79ff0ef          	jal	ra,a2e0b2 <upg_check_image_update_complete>
  a2e13e:	e81d                	bnez	s0,a2e174 <upg_set_complete_flag+0x82>
  a2e140:	e91d                	bnez	a0,a2e176 <upg_set_complete_flag+0x84>
  a2e142:	57e6                	lw	a5,120(sp)
  a2e144:	65c1                	lui	a1,0x10
  a2e146:	15fd                	addi	a1,a1,-1 # ffff <g_intheap_size+0x3>
  a2e148:	04b78d63          	beq	a5,a1,a2e1a2 <upg_set_complete_flag+0xb0>
  a2e14c:	c82e                	sw	a1,16(sp)
  a2e14e:	5a5a 5a5a 079f      	l.li	a5,0x5a5a5a5a
  a2e154:	4432                	lw	s0,12(sp)
  a2e156:	fcf583e3          	beq	a1,a5,a2e11c <upg_set_complete_flag+0x2a>
  a2e15a:	00a3 3338 051f      	l.li	a0,0xa33338
  a2e160:	ee5fd0ef          	jal	ra,a2c044 <print_str>
  a2e164:	4681                	li	a3,0
  a2e166:	0810                	addi	a2,sp,16
  a2e168:	4591                	li	a1,4
  a2e16a:	06440513          	addi	a0,s0,100
  a2e16e:	b30ff0ef          	jal	ra,a2d49e <upg_flash_write>
  a2e172:	b76d                	j	a2e11c <upg_set_complete_flag+0x2a>
  a2e174:	c911                	beqz	a0,a2e188 <upg_set_complete_flag+0x96>
  a2e176:	57e6                	lw	a5,120(sp)
  a2e178:	cb81                	beqz	a5,a2e188 <upg_set_complete_flag+0x96>
  a2e17a:	00a0 5ad4 079f      	l.li	a5,0xa05ad4
  a2e180:	f455                	bnez	s0,a2e12c <upg_set_complete_flag+0x3a>
  a2e182:	000782a3          	sb	zero,5(a5)
  a2e186:	b75d                	j	a2e12c <upg_set_complete_flag+0x3a>
  a2e188:	00a0 5ad4 079f      	l.li	a5,0xa05ad4
  a2e18e:	33dc                	lbu	a5,5(a5)
  a2e190:	5a5a 5a5a 059f      	l.li	a1,0x5a5a5a5a
  a2e196:	0de79dbb          	bnei	a5,13,a2e14c <upg_set_complete_flag+0x5a>
  a2e19a:	57e6                	lw	a5,120(sp)
  a2e19c:	dbc5                	beqz	a5,a2e14c <upg_set_complete_flag+0x5a>
  a2e19e:	4535                	li	a0,13
  a2e1a0:	b779                	j	a2e12e <upg_set_complete_flag+0x3c>
  a2e1a2:	5a5a 5a5a 059f      	l.li	a1,0x5a5a5a5a
  a2e1a8:	b755                	j	a2e14c <upg_set_complete_flag+0x5a>

00a2e1aa <upg_get_func_list>:
  a2e1aa:	00a0 5ad4 051f      	l.li	a0,0xa05ad4
  a2e1b0:	0521                	addi	a0,a0,8
  a2e1b2:	8082                	ret

00a2e1b4 <upg_is_inited>:
  a2e1b4:	00a0 5ad4 079f      	l.li	a5,0xa05ad4
  a2e1ba:	2388                	lbu	a0,0(a5)
  a2e1bc:	8082                	ret

00a2e1be <uapi_upg_init>:
  a2e1be:	8138                	push	{ra,s0-s1},-32
  a2e1c0:	00a06437          	lui	s0,0xa06
  a2e1c4:	ad444783          	lbu	a5,-1324(s0) # a05ad4 <g_upg_ctx>
  a2e1c8:	e7c9                	bnez	a5,a2e252 <uapi_upg_init+0x94>
  a2e1ca:	ad440413          	addi	s0,s0,-1324
  a2e1ce:	c519                	beqz	a0,a2e1dc <uapi_upg_init+0x1e>
  a2e1d0:	411c                	lw	a5,0(a0)
  a2e1d2:	c41c                	sw	a5,8(s0)
  a2e1d4:	415c                	lw	a5,4(a0)
  a2e1d6:	c45c                	sw	a5,12(s0)
  a2e1d8:	451c                	lw	a5,8(a0)
  a2e1da:	c81c                	sw	a5,16(s0)
  a2e1dc:	47b9                	li	a5,14
  a2e1de:	b05c                	sb	a5,5(s0)
  a2e1e0:	0068                	addi	a0,sp,12
  a2e1e2:	4785                	li	a5,1
  a2e1e4:	00042a23          	sw	zero,20(s0)
  a2e1e8:	a01c                	sb	a5,0(s0)
  a2e1ea:	000103a3          	sb	zero,7(sp)
  a2e1ee:	c402                	sw	zero,8(sp)
  a2e1f0:	c602                	sw	zero,12(sp)
  a2e1f2:	a95ff0ef          	jal	ra,a2dc86 <upg_alloc_and_get_upgrade_flag>
  a2e1f6:	448d                	li	s1,3
  a2e1f8:	e931                	bnez	a0,a2e24c <uapi_upg_init+0x8e>
  a2e1fa:	47b2                	lw	a5,12(sp)
  a2e1fc:	55aa 55aa 071f      	l.li	a4,0x55aa55aa
  a2e202:	4394                	lw	a3,0(a5)
  a2e204:	04e69063          	bne	a3,a4,a2e244 <uapi_upg_init+0x86>
  a2e208:	57b4                	lw	a3,104(a5)
  a2e20a:	aa55 aa55 071f      	l.li	a4,0xaa55aa55
  a2e210:	02e69a63          	bne	a3,a4,a2e244 <uapi_upg_init+0x86>
  a2e214:	53f8                	lw	a4,100(a5)
  a2e216:	4489                	li	s1,2
  a2e218:	e71d                	bnez	a4,a2e246 <uapi_upg_init+0x88>
  a2e21a:	04f7c703          	lbu	a4,79(a5)
  a2e21e:	c31d                	beqz	a4,a2e244 <uapi_upg_init+0x86>
  a2e220:	0028                	addi	a0,sp,8
  a2e222:	4ba4                	lw	s1,80(a5)
  a2e224:	91cff0ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a2e228:	ed11                	bnez	a0,a2e244 <uapi_upg_init+0x86>
  a2e22a:	4522                	lw	a0,8(sp)
  a2e22c:	4681                	li	a3,0
  a2e22e:	00710613          	addi	a2,sp,7
  a2e232:	04f50513          	addi	a0,a0,79
  a2e236:	4585                	li	a1,1
  a2e238:	c42a                	sw	a0,8(sp)
  a2e23a:	a64ff0ef          	jal	ra,a2d49e <upg_flash_write>
  a2e23e:	009034b3          	snez	s1,s1
  a2e242:	c111                	beqz	a0,a2e246 <uapi_upg_init+0x88>
  a2e244:	448d                	li	s1,3
  a2e246:	4532                	lw	a0,12(sp)
  a2e248:	a07ff0ef          	jal	ra,a2dc4e <upg_free>
  a2e24c:	a044                	sb	s1,4(s0)
  a2e24e:	4501                	li	a0,0
  a2e250:	8134                	popret	{ra,s0-s1},32
  a2e252:	8000 3041 051f      	l.li	a0,0x80003041
  a2e258:	bfe5                	j	a2e250 <uapi_upg_init+0x92>

00a2e25a <hash_operation.isra.1.constprop.11>:
  a2e25a:	8028                	push	{ra,s0},-16
  a2e25c:	4108                	lw	a0,0(a0)
  a2e25e:	02000693          	li	a3,32
  a2e262:	ef1fb0ef          	jal	ra,a2a152 <drv_rom_cipher_sha256>
  a2e266:	842a                	mv	s0,a0
  a2e268:	c519                	beqz	a0,a2e276 <hash_operation.isra.1.constprop.11+0x1c>
  a2e26a:	85aa                	mv	a1,a0
  a2e26c:	00a3 33b0 051f      	l.li	a0,0xa333b0
  a2e272:	dd3fd0ef          	jal	ra,a2c044 <print_str>
  a2e276:	8522                	mv	a0,s0
  a2e278:	8024                	popret	{ra,s0},16

00a2e27a <verify_hash_cmp>:
  a2e27a:	8018                	push	{ra},-16
  a2e27c:	a73fa0ef          	jal	ra,a28cee <memcmp>
  a2e280:	c901                	beqz	a0,a2e290 <verify_hash_cmp+0x16>
  a2e282:	00a3 36d4 051f      	l.li	a0,0xa336d4
  a2e288:	dbdfd0ef          	jal	ra,a2c044 <print_str>
  a2e28c:	557d                	li	a0,-1
  a2e28e:	8014                	popret	{ra},16
  a2e290:	4501                	li	a0,0
  a2e292:	bff5                	j	a2e28e <verify_hash_cmp+0x14>

00a2e294 <upg_verify_hash>:
  a2e294:	8548                	push	{ra,s0-s2},-96
  a2e296:	892a                	mv	s2,a0
  a2e298:	84ae                	mv	s1,a1
  a2e29a:	8432                	mv	s0,a2
  a2e29c:	4581                	li	a1,0
  a2e29e:	02000613          	li	a2,32
  a2e2a2:	1808                	addi	a0,sp,48
  a2e2a4:	c636                	sw	a3,12(sp)
  a2e2a6:	efefa0ef          	jal	ra,a289a4 <memset>
  a2e2aa:	4785                	li	a5,1
  a2e2ac:	1810                	addi	a2,sp,48
  a2e2ae:	85a6                	mv	a1,s1
  a2e2b0:	1028                	addi	a0,sp,40
  a2e2b2:	d44a                	sw	s2,40(sp)
  a2e2b4:	02f10623          	sb	a5,44(sp)
  a2e2b8:	fa3ff0ef          	jal	ra,a2e25a <hash_operation.isra.1.constprop.11>
  a2e2bc:	46b2                	lw	a3,12(sp)
  a2e2be:	c909                	beqz	a0,a2e2d0 <upg_verify_hash+0x3c>
  a2e2c0:	85aa                	mv	a1,a0
  a2e2c2:	00a3 3640 051f      	l.li	a0,0xa33640
  a2e2c8:	d7dfd0ef          	jal	ra,a2c044 <print_str>
  a2e2cc:	557d                	li	a0,-1
  a2e2ce:	8544                	popret	{ra,s0-s2},96
  a2e2d0:	8636                	mv	a2,a3
  a2e2d2:	180c                	addi	a1,sp,48
  a2e2d4:	8522                	mv	a0,s0
  a2e2d6:	fa5ff0ef          	jal	ra,a2e27a <verify_hash_cmp>
  a2e2da:	d975                	beqz	a0,a2e2ce <upg_verify_hash+0x3a>
  a2e2dc:	00a3 36d4 051f      	l.li	a0,0xa336d4
  a2e2e2:	d63fd0ef          	jal	ra,a2c044 <print_str>
  a2e2e6:	b7dd                	j	a2e2cc <upg_verify_hash+0x38>

00a2e2e8 <secure_authenticate>:
  a2e2e8:	8748                	push	{ra,s0-s2},-128
  a2e2ea:	57fd                	li	a5,-1
  a2e2ec:	c63e                	sw	a5,12(sp)
  a2e2ee:	c149                	beqz	a0,a2e370 <secure_authenticate+0x88>
  a2e2f0:	c5bd                	beqz	a1,a2e35e <secure_authenticate+0x76>
  a2e2f2:	c635                	beqz	a2,a2e35e <secure_authenticate+0x76>
  a2e2f4:	0884                	addi	s1,sp,80
  a2e2f6:	cc2a                	sw	a0,24(sp)
  a2e2f8:	d232                	sw	a2,36(sp)
  a2e2fa:	02050513          	addi	a0,a0,32
  a2e2fe:	02060613          	addi	a2,a2,32
  a2e302:	842e                	mv	s0,a1
  a2e304:	02000913          	li	s2,32
  a2e308:	ce2a                	sw	a0,28(sp)
  a2e30a:	d432                	sw	a2,40(sp)
  a2e30c:	02000693          	li	a3,32
  a2e310:	05a00613          	li	a2,90
  a2e314:	02000593          	li	a1,32
  a2e318:	8526                	mv	a0,s1
  a2e31a:	d04a                	sw	s2,32(sp)
  a2e31c:	d64a                	sw	s2,44(sp)
  a2e31e:	6ce020ef          	jal	ra,a309ec <memset_s>
  a2e322:	405c                	lw	a5,4(s0)
  a2e324:	400c                	lw	a1,0(s0)
  a2e326:	8626                	mv	a2,s1
  a2e328:	c4be                	sw	a5,72(sp)
  a2e32a:	00a8                	addi	a0,sp,72
  a2e32c:	4785                	li	a5,1
  a2e32e:	ca26                	sw	s1,20(sp)
  a2e330:	c84a                	sw	s2,16(sp)
  a2e332:	04f10623          	sb	a5,76(sp)
  a2e336:	f25ff0ef          	jal	ra,a2e25a <hash_operation.isra.1.constprop.11>
  a2e33a:	842a                	mv	s0,a0
  a2e33c:	c11d                	beqz	a0,a2e362 <secure_authenticate+0x7a>
  a2e33e:	85aa                	mv	a1,a0
  a2e340:	00a3 33f0 051f      	l.li	a0,0xa333f0
  a2e346:	cfffd0ef          	jal	ra,a2c044 <print_str>
  a2e34a:	c622                	sw	s0,12(sp)
  a2e34c:	47b2                	lw	a5,12(sp)
  a2e34e:	c799                	beqz	a5,a2e35c <secure_authenticate+0x74>
  a2e350:	45b2                	lw	a1,12(sp)
  a2e352:	00a3 3414 051f      	l.li	a0,0xa33414
  a2e358:	cedfd0ef          	jal	ra,a2c044 <print_str>
  a2e35c:	47b2                	lw	a5,12(sp)
  a2e35e:	853e                	mv	a0,a5
  a2e360:	8744                	popret	{ra,s0-s2},128
  a2e362:	1050                	addi	a2,sp,36
  a2e364:	080c                	addi	a1,sp,16
  a2e366:	0828                	addi	a0,sp,24
  a2e368:	de3fb0ef          	jal	ra,a2a14a <drv_rom_cipher_pke_bp256r_verify>
  a2e36c:	842a                	mv	s0,a0
  a2e36e:	bff1                	j	a2e34a <secure_authenticate+0x62>
  a2e370:	57fd                	li	a5,-1
  a2e372:	b7f5                	j	a2e35e <secure_authenticate+0x76>

00a2e374 <verify_fota_info.isra.4>:
  a2e374:	8238                	push	{ra,s0-s1},-48
  a2e376:	455c                	lw	a5,12(a0)
  a2e378:	842a                	mv	s0,a0
  a2e37a:	0c050613          	addi	a2,a0,192
  a2e37e:	200798bb          	bnei	a5,32,a2e3a0 <verify_fota_info.isra.4+0x2c>
  a2e382:	00a3 367c 051f      	l.li	a0,0xa3367c
  a2e388:	c632                	sw	a2,12(sp)
  a2e38a:	cbbfd0ef          	jal	ra,a2c044 <print_str>
  a2e38e:	4632                	lw	a2,12(sp)
  a2e390:	02000693          	li	a3,32
  a2e394:	0c000593          	li	a1,192
  a2e398:	8522                	mv	a0,s0
  a2e39a:	efbff0ef          	jal	ra,a2e294 <upg_verify_hash>
  a2e39e:	8234                	popret	{ra,s0-s1},48
  a2e3a0:	00a3 36a8 051f      	l.li	a0,0xa336a8
  a2e3a6:	84ae                	mv	s1,a1
  a2e3a8:	c632                	sw	a2,12(sp)
  a2e3aa:	c9bfd0ef          	jal	ra,a2c044 <print_str>
  a2e3ae:	4632                	lw	a2,12(sp)
  a2e3b0:	0c000793          	li	a5,192
  a2e3b4:	082c                	addi	a1,sp,24
  a2e3b6:	8526                	mv	a0,s1
  a2e3b8:	ce22                	sw	s0,28(sp)
  a2e3ba:	cc3e                	sw	a5,24(sp)
  a2e3bc:	f2dff0ef          	jal	ra,a2e2e8 <secure_authenticate>
  a2e3c0:	bff9                	j	a2e39e <verify_fota_info.isra.4+0x2a>

00a2e3c2 <uapi_upg_verify_file_head>:
  a2e3c2:	8238                	push	{ra,s0-s1},-48
  a2e3c4:	84aa                	mv	s1,a0
  a2e3c6:	defff0ef          	jal	ra,a2e1b4 <upg_is_inited>
  a2e3ca:	c169                	beqz	a0,a2e48c <uapi_upg_verify_file_head+0xca>
  a2e3cc:	902ff0ef          	jal	ra,a2d4ce <upg_get_root_public_key>
  a2e3d0:	c171                	beqz	a0,a2e494 <uapi_upg_verify_file_head+0xd2>
  a2e3d2:	409c                	lw	a5,0(s1)
  a2e3d4:	cb8d 154e 071f      	l.li	a4,0xcb8d154e
  a2e3da:	04e79263          	bne	a5,a4,a2e41e <uapi_upg_verify_file_head+0x5c>
  a2e3de:	1004a703          	lw	a4,256(s1)
  a2e3e2:	02f71e63          	bne	a4,a5,a2e41e <uapi_upg_verify_file_head+0x5c>
  a2e3e6:	44dc                	lw	a5,12(s1)
  a2e3e8:	0c048613          	addi	a2,s1,192
  a2e3ec:	201790bb          	bnei	a5,32,a2e42e <uapi_upg_verify_file_head+0x6c>
  a2e3f0:	00a3 3500 051f      	l.li	a0,0xa33500
  a2e3f6:	c632                	sw	a2,12(sp)
  a2e3f8:	c4dfd0ef          	jal	ra,a2c044 <print_str>
  a2e3fc:	4632                	lw	a2,12(sp)
  a2e3fe:	02000693          	li	a3,32
  a2e402:	0c000593          	li	a1,192
  a2e406:	8526                	mv	a0,s1
  a2e408:	e8dff0ef          	jal	ra,a2e294 <upg_verify_hash>
  a2e40c:	842a                	mv	s0,a0
  a2e40e:	c905                	beqz	a0,a2e43e <uapi_upg_verify_file_head+0x7c>
  a2e410:	85aa                	mv	a1,a0
  a2e412:	00a3 3530 051f      	l.li	a0,0xa33530
  a2e418:	c2dfd0ef          	jal	ra,a2c044 <print_str>
  a2e41c:	a039                	j	a2e42a <uapi_upg_verify_file_head+0x68>
  a2e41e:	00a3 34dc 051f      	l.li	a0,0xa334dc
  a2e424:	c21fd0ef          	jal	ra,a2c044 <print_str>
  a2e428:	547d                	li	s0,-1
  a2e42a:	8522                	mv	a0,s0
  a2e42c:	8234                	popret	{ra,s0-s1},48
  a2e42e:	0c000793          	li	a5,192
  a2e432:	082c                	addi	a1,sp,24
  a2e434:	ce26                	sw	s1,28(sp)
  a2e436:	cc3e                	sw	a5,24(sp)
  a2e438:	eb1ff0ef          	jal	ra,a2e2e8 <secure_authenticate>
  a2e43c:	bfc1                	j	a2e40c <uapi_upg_verify_file_head+0x4a>
  a2e43e:	08048593          	addi	a1,s1,128
  a2e442:	10048513          	addi	a0,s1,256
  a2e446:	f2fff0ef          	jal	ra,a2e374 <verify_fota_info.isra.4>
  a2e44a:	842a                	mv	s0,a0
  a2e44c:	c511                	beqz	a0,a2e458 <uapi_upg_verify_file_head+0x96>
  a2e44e:	85aa                	mv	a1,a0
  a2e450:	00a3 3560 051f      	l.li	a0,0xa33560
  a2e456:	b7c9                	j	a2e418 <uapi_upg_verify_file_head+0x56>
  a2e458:	8526                	mv	a0,s1
  a2e45a:	892ff0ef          	jal	ra,a2d4ec <upg_check_fota_information>
  a2e45e:	842a                	mv	s0,a0
  a2e460:	c511                	beqz	a0,a2e46c <uapi_upg_verify_file_head+0xaa>
  a2e462:	85aa                	mv	a1,a0
  a2e464:	00a3 3590 051f      	l.li	a0,0xa33590
  a2e46a:	b77d                	j	a2e418 <uapi_upg_verify_file_head+0x56>
  a2e46c:	00a0 5af8 079f      	l.li	a5,0xa05af8
  a2e472:	439c                	lw	a5,0(a5)
  a2e474:	dbdd                	beqz	a5,a2e42a <uapi_upg_verify_file_head+0x68>
  a2e476:	00a0 5afc 071f      	l.li	a4,0xa05afc
  a2e47c:	4310                	lw	a2,0(a4)
  a2e47e:	07000593          	li	a1,112
  a2e482:	15048513          	addi	a0,s1,336
  a2e486:	9782                	jalr	a5
  a2e488:	842a                	mv	s0,a0
  a2e48a:	b745                	j	a2e42a <uapi_upg_verify_file_head+0x68>
  a2e48c:	8000 3040 041f      	l.li	s0,0x80003040
  a2e492:	bf61                	j	a2e42a <uapi_upg_verify_file_head+0x68>
  a2e494:	8000 3063 041f      	l.li	s0,0x80003063
  a2e49a:	bf41                	j	a2e42a <uapi_upg_verify_file_head+0x68>

00a2e49c <uapi_upg_verify_file_image>:
  a2e49c:	8148                	push	{ra,s0-s2},-32
  a2e49e:	411c                	lw	a5,0(a0)
  a2e4a0:	464f 5451 071f      	l.li	a4,0x464f5451
  a2e4a6:	00e78e63          	beq	a5,a4,a2e4c2 <uapi_upg_verify_file_image+0x26>
  a2e4aa:	85be                	mv	a1,a5
  a2e4ac:	00a3 35cc 051f      	l.li	a0,0xa335cc
  a2e4b2:	b93fd0ef          	jal	ra,a2c044 <print_str>
  a2e4b6:	450d                	li	a0,3
  a2e4b8:	bd1ff0ef          	jal	ra,a2e088 <upg_set_temporary_result>
  a2e4bc:	547d                	li	s0,-1
  a2e4be:	8522                	mv	a0,s0
  a2e4c0:	8144                	popret	{ra,s0-s2},32
  a2e4c2:	86b2                	mv	a3,a2
  a2e4c4:	862e                	mv	a2,a1
  a2e4c6:	0a000593          	li	a1,160
  a2e4ca:	84aa                	mv	s1,a0
  a2e4cc:	dc9ff0ef          	jal	ra,a2e294 <upg_verify_hash>
  a2e4d0:	842a                	mv	s0,a0
  a2e4d2:	c509                	beqz	a0,a2e4dc <uapi_upg_verify_file_image+0x40>
  a2e4d4:	450d                	li	a0,3
  a2e4d6:	bb3ff0ef          	jal	ra,a2e088 <upg_set_temporary_result>
  a2e4da:	b7d5                	j	a2e4be <uapi_upg_verify_file_image+0x22>
  a2e4dc:	44dc                	lw	a5,12(s1)
  a2e4de:	0034                	addi	a3,sp,8
  a2e4e0:	0070                	addi	a2,sp,12
  a2e4e2:	07bd                	addi	a5,a5,15
  a2e4e4:	9bc1                	andi	a5,a5,-16
  a2e4e6:	4581                	li	a1,0
  a2e4e8:	8526                	mv	a0,s1
  a2e4ea:	c402                	sw	zero,8(sp)
  a2e4ec:	c63e                	sw	a5,12(sp)
  a2e4ee:	981ff0ef          	jal	ra,a2de6e <upg_get_pkg_image_data>
  a2e4f2:	842a                	mv	s0,a0
  a2e4f4:	e905                	bnez	a0,a2e524 <uapi_upg_verify_file_image+0x88>
  a2e4f6:	4522                	lw	a0,8(sp)
  a2e4f8:	e901                	bnez	a0,a2e508 <uapi_upg_verify_file_image+0x6c>
  a2e4fa:	40cc                	lw	a1,4(s1)
  a2e4fc:	00a3 360c 051f      	l.li	a0,0xa3360c
  a2e502:	b43fd0ef          	jal	ra,a2c044 <print_str>
  a2e506:	bf65                	j	a2e4be <uapi_upg_verify_file_image+0x22>
  a2e508:	45b2                	lw	a1,12(sp)
  a2e50a:	02000693          	li	a3,32
  a2e50e:	01048613          	addi	a2,s1,16
  a2e512:	d83ff0ef          	jal	ra,a2e294 <upg_verify_hash>
  a2e516:	892a                	mv	s2,a0
  a2e518:	d16d                	beqz	a0,a2e4fa <uapi_upg_verify_file_image+0x5e>
  a2e51a:	450d                	li	a0,3
  a2e51c:	b6dff0ef          	jal	ra,a2e088 <upg_set_temporary_result>
  a2e520:	844a                	mv	s0,s2
  a2e522:	bf71                	j	a2e4be <uapi_upg_verify_file_image+0x22>
  a2e524:	892a                	mv	s2,a0
  a2e526:	bfd5                	j	a2e51a <uapi_upg_verify_file_image+0x7e>

00a2e528 <uapi_upg_verify_file>:
  a2e528:	8158                	push	{ra,s0-s3},-48
  a2e52a:	892a                	mv	s2,a0
  a2e52c:	c402                	sw	zero,8(sp)
  a2e52e:	c87ff0ef          	jal	ra,a2e1b4 <upg_is_inited>
  a2e532:	c161                	beqz	a0,a2e5f2 <uapi_upg_verify_file+0xca>
  a2e534:	854a                	mv	a0,s2
  a2e536:	e8dff0ef          	jal	ra,a2e3c2 <uapi_upg_verify_file_head>
  a2e53a:	842a                	mv	s0,a0
  a2e53c:	c511                	beqz	a0,a2e548 <uapi_upg_verify_file+0x20>
  a2e53e:	4505                	li	a0,1
  a2e540:	b49ff0ef          	jal	ra,a2e088 <upg_set_temporary_result>
  a2e544:	8522                	mv	a0,s0
  a2e546:	8154                	popret	{ra,s0-s3},48
  a2e548:	00a3 3444 051f      	l.li	a0,0xa33444
  a2e54e:	af7fd0ef          	jal	ra,a2c044 <print_str>
  a2e552:	002c                	addi	a1,sp,8
  a2e554:	854a                	mv	a0,s2
  a2e556:	8b7ff0ef          	jal	ra,a2de0c <upg_get_pkg_image_hash_table>
  a2e55a:	842a                	mv	s0,a0
  a2e55c:	e119                	bnez	a0,a2e562 <uapi_upg_verify_file+0x3a>
  a2e55e:	4522                	lw	a0,8(sp)
  a2e560:	e519                	bnez	a0,a2e56e <uapi_upg_verify_file+0x46>
  a2e562:	00a3 3460 051f      	l.li	a0,0xa33460
  a2e568:	addfd0ef          	jal	ra,a2c044 <print_str>
  a2e56c:	bfe1                	j	a2e544 <uapi_upg_verify_file+0x1c>
  a2e56e:	12492583          	lw	a1,292(s2)
  a2e572:	02000693          	li	a3,32
  a2e576:	12890613          	addi	a2,s2,296
  a2e57a:	d1bff0ef          	jal	ra,a2e294 <upg_verify_hash>
  a2e57e:	842a                	mv	s0,a0
  a2e580:	c901                	beqz	a0,a2e590 <uapi_upg_verify_file+0x68>
  a2e582:	4509                	li	a0,2
  a2e584:	b05ff0ef          	jal	ra,a2e088 <upg_set_temporary_result>
  a2e588:	4522                	lw	a0,8(sp)
  a2e58a:	ec4ff0ef          	jal	ra,a2dc4e <upg_free>
  a2e58e:	bf5d                	j	a2e544 <uapi_upg_verify_file+0x1c>
  a2e590:	00a3 348c 051f      	l.li	a0,0xa3348c
  a2e596:	aaffd0ef          	jal	ra,a2c044 <print_str>
  a2e59a:	44a2                	lw	s1,8(sp)
  a2e59c:	c602                	sw	zero,12(sp)
  a2e59e:	4981                	li	s3,0
  a2e5a0:	04b1                	addi	s1,s1,12
  a2e5a2:	14892783          	lw	a5,328(s2)
  a2e5a6:	00f9e463          	bltu	s3,a5,a2e5ae <uapi_upg_verify_file+0x86>
  a2e5aa:	4401                	li	s0,0
  a2e5ac:	a839                	j	a2e5ca <uapi_upg_verify_file+0xa2>
  a2e5ae:	006c                	addi	a1,sp,12
  a2e5b0:	ff448513          	addi	a0,s1,-12
  a2e5b4:	88dff0ef          	jal	ra,a2de40 <upg_get_pkg_image_header>
  a2e5b8:	842a                	mv	s0,a0
  a2e5ba:	e119                	bnez	a0,a2e5c0 <uapi_upg_verify_file+0x98>
  a2e5bc:	4532                	lw	a0,12(sp)
  a2e5be:	e911                	bnez	a0,a2e5d2 <uapi_upg_verify_file+0xaa>
  a2e5c0:	00a3 34b4 051f      	l.li	a0,0xa334b4
  a2e5c6:	a7ffd0ef          	jal	ra,a2c044 <print_str>
  a2e5ca:	4532                	lw	a0,12(sp)
  a2e5cc:	e82ff0ef          	jal	ra,a2dc4e <upg_free>
  a2e5d0:	bf65                	j	a2e588 <uapi_upg_verify_file+0x60>
  a2e5d2:	85a6                	mv	a1,s1
  a2e5d4:	4685                	li	a3,1
  a2e5d6:	02000613          	li	a2,32
  a2e5da:	ec3ff0ef          	jal	ra,a2e49c <uapi_upg_verify_file_image>
  a2e5de:	842a                	mv	s0,a0
  a2e5e0:	02c48493          	addi	s1,s1,44
  a2e5e4:	f17d                	bnez	a0,a2e5ca <uapi_upg_verify_file+0xa2>
  a2e5e6:	4532                	lw	a0,12(sp)
  a2e5e8:	0985                	addi	s3,s3,1
  a2e5ea:	e64ff0ef          	jal	ra,a2dc4e <upg_free>
  a2e5ee:	c602                	sw	zero,12(sp)
  a2e5f0:	bf4d                	j	a2e5a2 <uapi_upg_verify_file+0x7a>
  a2e5f2:	8000 3040 041f      	l.li	s0,0x80003040
  a2e5f8:	b7b1                	j	a2e544 <uapi_upg_verify_file+0x1c>

00a2e5fa <uapi_upg_reset_upgrade_flag>:
  a2e5fa:	8218                	push	{ra},-48
  a2e5fc:	c602                	sw	zero,12(sp)
  a2e5fe:	bb7ff0ef          	jal	ra,a2e1b4 <upg_is_inited>
  a2e602:	c13d                	beqz	a0,a2e668 <uapi_upg_reset_upgrade_flag+0x6e>
  a2e604:	080c                	addi	a1,sp,16
  a2e606:	0068                	addi	a0,sp,12
  a2e608:	d57fe0ef          	jal	ra,a2d35e <upg_get_progress_status_start_addr>
  a2e60c:	ed29                	bnez	a0,a2e666 <uapi_upg_reset_upgrade_flag+0x6c>
  a2e60e:	0028                	addi	a0,sp,8
  a2e610:	d31fe0ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a2e614:	e929                	bnez	a0,a2e666 <uapi_upg_reset_upgrade_flag+0x6c>
  a2e616:	4522                	lw	a0,8(sp)
  a2e618:	0850                	addi	a2,sp,20
  a2e61a:	4591                	li	a1,4
  a2e61c:	0521                	addi	a0,a0,8
  a2e61e:	ca02                	sw	zero,20(sp)
  a2e620:	dbbfe0ef          	jal	ra,a2d3da <upg_flash_read>
  a2e624:	e129                	bnez	a0,a2e666 <uapi_upg_reset_upgrade_flag+0x6c>
  a2e626:	45c2                	lw	a1,16(sp)
  a2e628:	4532                	lw	a0,12(sp)
  a2e62a:	dc7fe0ef          	jal	ra,a2d3f0 <upg_flash_erase>
  a2e62e:	ed05                	bnez	a0,a2e666 <uapi_upg_reset_upgrade_flag+0x6c>
  a2e630:	4522                	lw	a0,8(sp)
  a2e632:	4681                	li	a3,0
  a2e634:	0830                	addi	a2,sp,24
  a2e636:	4591                	li	a1,4
  a2e638:	0511                	addi	a0,a0,4
  a2e63a:	cc02                	sw	zero,24(sp)
  a2e63c:	e63fe0ef          	jal	ra,a2d49e <upg_flash_write>
  a2e640:	e11d                	bnez	a0,a2e666 <uapi_upg_reset_upgrade_flag+0x6c>
  a2e642:	4522                	lw	a0,8(sp)
  a2e644:	55aa 55aa 079f      	l.li	a5,0x55aa55aa
  a2e64a:	4681                	li	a3,0
  a2e64c:	0870                	addi	a2,sp,28
  a2e64e:	4591                	li	a1,4
  a2e650:	ce3e                	sw	a5,28(sp)
  a2e652:	e4dfe0ef          	jal	ra,a2d49e <upg_flash_write>
  a2e656:	e901                	bnez	a0,a2e666 <uapi_upg_reset_upgrade_flag+0x6c>
  a2e658:	4522                	lw	a0,8(sp)
  a2e65a:	4681                	li	a3,0
  a2e65c:	0850                	addi	a2,sp,20
  a2e65e:	4591                	li	a1,4
  a2e660:	0521                	addi	a0,a0,8
  a2e662:	e3dfe0ef          	jal	ra,a2d49e <upg_flash_write>
  a2e666:	8214                	popret	{ra},48
  a2e668:	8000 3040 051f      	l.li	a0,0x80003040
  a2e66e:	bfe5                	j	a2e666 <uapi_upg_reset_upgrade_flag+0x6c>

00a2e670 <uapi_upg_request_upgrade>:
  a2e670:	8148                	push	{ra,s0-s2},-32
  a2e672:	892a                	mv	s2,a0
  a2e674:	c002                	sw	zero,0(sp)
  a2e676:	b3fff0ef          	jal	ra,a2e1b4 <upg_is_inited>
  a2e67a:	0e050363          	beqz	a0,a2e760 <uapi_upg_request_upgrade+0xf0>
  a2e67e:	850a                	mv	a0,sp
  a2e680:	f6aff0ef          	jal	ra,a2ddea <upg_get_package_header>
  a2e684:	842a                	mv	s0,a0
  a2e686:	e119                	bnez	a0,a2e68c <uapi_upg_request_upgrade+0x1c>
  a2e688:	4502                	lw	a0,0(sp)
  a2e68a:	e901                	bnez	a0,a2e69a <uapi_upg_request_upgrade+0x2a>
  a2e68c:	00a3 3700 051f      	l.li	a0,0xa33700
  a2e692:	9b3fd0ef          	jal	ra,a2c044 <print_str>
  a2e696:	8522                	mv	a0,s0
  a2e698:	8144                	popret	{ra,s0-s2},32
  a2e69a:	e8fff0ef          	jal	ra,a2e528 <uapi_upg_verify_file>
  a2e69e:	842a                	mv	s0,a0
  a2e6a0:	4502                	lw	a0,0(sp)
  a2e6a2:	c811                	beqz	s0,a2e6b6 <uapi_upg_request_upgrade+0x46>
  a2e6a4:	daaff0ef          	jal	ra,a2dc4e <upg_free>
  a2e6a8:	85a2                	mv	a1,s0
  a2e6aa:	00a3 3724 051f      	l.li	a0,0xa33724
  a2e6b0:	995fd0ef          	jal	ra,a2c044 <print_str>
  a2e6b4:	b7cd                	j	a2e696 <uapi_upg_request_upgrade+0x26>
  a2e6b6:	c602                	sw	zero,12(sp)
  a2e6b8:	14852483          	lw	s1,328(a0)
  a2e6bc:	1504e6bb          	bltui	s1,21,a2e6d6 <uapi_upg_request_upgrade+0x66>
  a2e6c0:	8000 3043 041f      	l.li	s0,0x80003043
  a2e6c6:	4502                	lw	a0,0(sp)
  a2e6c8:	d86ff0ef          	jal	ra,a2dc4e <upg_free>
  a2e6cc:	85a2                	mv	a1,s0
  a2e6ce:	00a3 3754 051f      	l.li	a0,0xa33754
  a2e6d4:	bff1                	j	a2e6b0 <uapi_upg_request_upgrade+0x40>
  a2e6d6:	006c                	addi	a1,sp,12
  a2e6d8:	f34ff0ef          	jal	ra,a2de0c <upg_get_pkg_image_hash_table>
  a2e6dc:	842a                	mv	s0,a0
  a2e6de:	ed11                	bnez	a0,a2e6fa <uapi_upg_request_upgrade+0x8a>
  a2e6e0:	4532                	lw	a0,12(sp)
  a2e6e2:	cd01                	beqz	a0,a2e6fa <uapi_upg_request_upgrade+0x8a>
  a2e6e4:	87aa                	mv	a5,a0
  a2e6e6:	2c9514db          	muliadd	s1,a0,s1,44
  a2e6ea:	cb9e 063c 071f      	l.li	a4,0xcb9e063c
  a2e6f0:	06979163          	bne	a5,s1,a2e752 <uapi_upg_request_upgrade+0xe2>
  a2e6f4:	d5aff0ef          	jal	ra,a2dc4e <upg_free>
  a2e6f8:	a039                	j	a2e706 <uapi_upg_request_upgrade+0x96>
  a2e6fa:	00a3 3460 051f      	l.li	a0,0xa33460
  a2e700:	945fd0ef          	jal	ra,a2c044 <print_str>
  a2e704:	f069                	bnez	s0,a2e6c6 <uapi_upg_request_upgrade+0x56>
  a2e706:	4502                	lw	a0,0(sp)
  a2e708:	d46ff0ef          	jal	ra,a2dc4e <upg_free>
  a2e70c:	0028                	addi	a0,sp,8
  a2e70e:	c222                	sw	s0,4(sp)
  a2e710:	c402                	sw	zero,8(sp)
  a2e712:	c2ffe0ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a2e716:	842a                	mv	s0,a0
  a2e718:	e921                	bnez	a0,a2e768 <uapi_upg_request_upgrade+0xf8>
  a2e71a:	4522                	lw	a0,8(sp)
  a2e71c:	4681                	li	a3,0
  a2e71e:	0050                	addi	a2,sp,4
  a2e720:	4591                	li	a1,4
  a2e722:	0531                	addi	a0,a0,12
  a2e724:	d7bfe0ef          	jal	ra,a2d49e <upg_flash_write>
  a2e728:	842a                	mv	s0,a0
  a2e72a:	ed1d                	bnez	a0,a2e768 <uapi_upg_request_upgrade+0xf8>
  a2e72c:	4522                	lw	a0,8(sp)
  a2e72e:	aa55 aa55 079f      	l.li	a5,0xaa55aa55
  a2e734:	4681                	li	a3,0
  a2e736:	0070                	addi	a2,sp,12
  a2e738:	4591                	li	a1,4
  a2e73a:	06850513          	addi	a0,a0,104
  a2e73e:	c63e                	sw	a5,12(sp)
  a2e740:	d5ffe0ef          	jal	ra,a2d49e <upg_flash_write>
  a2e744:	842a                	mv	s0,a0
  a2e746:	e10d                	bnez	a0,a2e768 <uapi_upg_request_upgrade+0xf8>
  a2e748:	f40907e3          	beqz	s2,a2e696 <uapi_upg_request_upgrade+0x26>
  a2e74c:	c53fe0ef          	jal	ra,a2d39e <upg_reboot>
  a2e750:	b799                	j	a2e696 <uapi_upg_request_upgrade+0x26>
  a2e752:	4394                	lw	a3,0(a5)
  a2e754:	00e68363          	beq	a3,a4,a2e75a <uapi_upg_request_upgrade+0xea>
  a2e758:	0405                	addi	s0,s0,1
  a2e75a:	02c78793          	addi	a5,a5,44
  a2e75e:	bf49                	j	a2e6f0 <uapi_upg_request_upgrade+0x80>
  a2e760:	8000 3040 041f      	l.li	s0,0x80003040
  a2e766:	bf05                	j	a2e696 <uapi_upg_request_upgrade+0x26>
  a2e768:	85a2                	mv	a1,s0
  a2e76a:	00a3 3790 051f      	l.li	a0,0xa33790
  a2e770:	b781                	j	a2e6b0 <uapi_upg_request_upgrade+0x40>

00a2e772 <upg_encry_and_write_pkt>:
  a2e772:	8158                	push	{ra,s0-s3},-48
  a2e774:	51f4                	lw	a3,100(a1)
  a2e776:	c402                	sw	zero,8(sp)
  a2e778:	c602                	sw	zero,12(sp)
  a2e77a:	3c78 96e1 071f      	l.li	a4,0x3c7896e1
  a2e780:	08e69563          	bne	a3,a4,a2e80a <upg_encry_and_write_pkt+0x98>
  a2e784:	892e                	mv	s2,a1
  a2e786:	842a                	mv	s0,a0
  a2e788:	85b2                	mv	a1,a2
  a2e78a:	0028                	addi	a0,sp,8
  a2e78c:	f11fe0ef          	jal	ra,a2d69c <upg_process_cryto_info>
  a2e790:	84aa                	mv	s1,a0
  a2e792:	e905                	bnez	a0,a2e7c2 <upg_encry_and_write_pkt+0x50>
  a2e794:	4c5c                	lw	a5,28(s0)
  a2e796:	00f7f993          	andi	s3,a5,15
  a2e79a:	9bc1                	andi	a5,a5,-16
  a2e79c:	c63e                	sw	a5,12(sp)
  a2e79e:	c3a1                	beqz	a5,a2e7de <upg_encry_and_write_pkt+0x6c>
  a2e7a0:	4722                	lw	a4,8(sp)
  a2e7a2:	4c08                	lw	a0,24(s0)
  a2e7a4:	864a                	mv	a2,s2
  a2e7a6:	40e785b3          	sub	a1,a5,a4
  a2e7aa:	953a                	add	a0,a0,a4
  a2e7ac:	f49fe0ef          	jal	ra,a2d6f4 <upg_encry_fota_pkt>
  a2e7b0:	892a                	mv	s2,a0
  a2e7b2:	c911                	beqz	a0,a2e7c6 <upg_encry_and_write_pkt+0x54>
  a2e7b4:	85aa                	mv	a1,a0
  a2e7b6:	00a3 31fc 051f      	l.li	a0,0xa331fc
  a2e7bc:	889fd0ef          	jal	ra,a2c044 <print_str>
  a2e7c0:	84ca                	mv	s1,s2
  a2e7c2:	8526                	mv	a0,s1
  a2e7c4:	8154                	popret	{ra,s0-s3},48
  a2e7c6:	4014                	lw	a3,0(s0)
  a2e7c8:	4c0c                	lw	a1,24(s0)
  a2e7ca:	4408                	lw	a0,8(s0)
  a2e7cc:	4701                	li	a4,0
  a2e7ce:	0070                	addi	a2,sp,12
  a2e7d0:	790000ef          	jal	ra,a2ef60 <upg_write_new_image_data>
  a2e7d4:	ed0d                	bnez	a0,a2e80e <upg_encry_and_write_pkt+0x9c>
  a2e7d6:	441c                	lw	a5,8(s0)
  a2e7d8:	4732                	lw	a4,12(sp)
  a2e7da:	97ba                	add	a5,a5,a4
  a2e7dc:	c41c                	sw	a5,8(s0)
  a2e7de:	02098463          	beqz	s3,a2e806 <upg_encry_and_write_pkt+0x94>
  a2e7e2:	4c08                	lw	a0,24(s0)
  a2e7e4:	4632                	lw	a2,12(sp)
  a2e7e6:	86ce                	mv	a3,s3
  a2e7e8:	6591                	lui	a1,0x4
  a2e7ea:	962a                	add	a2,a2,a0
  a2e7ec:	1bc020ef          	jal	ra,a309a8 <memmove_s>
  a2e7f0:	c901                	beqz	a0,a2e800 <upg_encry_and_write_pkt+0x8e>
  a2e7f2:	00a3 37c0 051f      	l.li	a0,0xa337c0
  a2e7f8:	84dfd0ef          	jal	ra,a2c044 <print_str>
  a2e7fc:	54fd                	li	s1,-1
  a2e7fe:	b7d1                	j	a2e7c2 <upg_encry_and_write_pkt+0x50>
  a2e800:	01342e23          	sw	s3,28(s0)
  a2e804:	bf7d                	j	a2e7c2 <upg_encry_and_write_pkt+0x50>
  a2e806:	00042e23          	sw	zero,28(s0)
  a2e80a:	4481                	li	s1,0
  a2e80c:	bf5d                	j	a2e7c2 <upg_encry_and_write_pkt+0x50>
  a2e80e:	4485                	li	s1,1
  a2e810:	bf4d                	j	a2e7c2 <upg_encry_and_write_pkt+0x50>

00a2e812 <upg_lzma_alloc>:
  a2e812:	8128                	push	{ra,s0},-32
  a2e814:	852e                	mv	a0,a1
  a2e816:	c62e                	sw	a1,12(sp)
  a2e818:	c1cff0ef          	jal	ra,a2dc34 <upg_malloc>
  a2e81c:	842a                	mv	s0,a0
  a2e81e:	45b2                	lw	a1,12(sp)
  a2e820:	e511                	bnez	a0,a2e82c <upg_lzma_alloc+0x1a>
  a2e822:	00a3 37f0 051f      	l.li	a0,0xa337f0
  a2e828:	81dfd0ef          	jal	ra,a2c044 <print_str>
  a2e82c:	8522                	mv	a0,s0
  a2e82e:	8124                	popret	{ra,s0},32

00a2e830 <upg_lzma_free>:
  a2e830:	c581                	beqz	a1,a2e838 <upg_lzma_free+0x8>
  a2e832:	852e                	mv	a0,a1
  a2e834:	c1aff06f          	j	a2dc4e <upg_free>
  a2e838:	8082                	ret

00a2e83a <upg_lzma_init_buf>:
  a2e83a:	8128                	push	{ra,s0},-32
  a2e83c:	842a                	mv	s0,a0
  a2e83e:	02052023          	sw	zero,32(a0)
  a2e842:	00052e23          	sw	zero,28(a0)
  a2e846:	6585                	lui	a1,0x1
  a2e848:	0028                	addi	a0,sp,8
  a2e84a:	fc9ff0ef          	jal	ra,a2e812 <upg_lzma_alloc>
  a2e84e:	c848                	sw	a0,20(s0)
  a2e850:	cd11                	beqz	a0,a2e86c <upg_lzma_init_buf+0x32>
  a2e852:	6591                	lui	a1,0x4
  a2e854:	0028                	addi	a0,sp,8
  a2e856:	fbdff0ef          	jal	ra,a2e812 <upg_lzma_alloc>
  a2e85a:	cc08                	sw	a0,24(s0)
  a2e85c:	4781                	li	a5,0
  a2e85e:	e911                	bnez	a0,a2e872 <upg_lzma_init_buf+0x38>
  a2e860:	484c                	lw	a1,20(s0)
  a2e862:	0028                	addi	a0,sp,8
  a2e864:	fcdff0ef          	jal	ra,a2e830 <upg_lzma_free>
  a2e868:	00042a23          	sw	zero,20(s0)
  a2e86c:	800007b7          	lui	a5,0x80000
  a2e870:	0795                	addi	a5,a5,5 # 80000005 <_gp_+0x7f5cb849>
  a2e872:	853e                	mv	a0,a5
  a2e874:	8124                	popret	{ra,s0},32

00a2e876 <upg_lzma_init>:
  a2e876:	8168                	push	{ra,s0-s4},-48
  a2e878:	00a2 e812 079f      	l.li	a5,0xa2e812
  a2e87e:	c43e                	sw	a5,8(sp)
  a2e880:	00a2 e830 079f      	l.li	a5,0xa2e830
  a2e886:	00052823          	sw	zero,16(a0)
  a2e88a:	00052423          	sw	zero,8(a0)
  a2e88e:	c63e                	sw	a5,12(sp)
  a2e890:	0506f3bb          	bgeui	a3,5,a2e89e <upg_lzma_init+0x28>
  a2e894:	8000 3042 041f      	l.li	s0,0x80003042
  a2e89a:	8522                	mv	a0,s0
  a2e89c:	8164                	popret	{ra,s0-s4},48
  a2e89e:	89b2                	mv	s3,a2
  a2e8a0:	8436                	mv	s0,a3
  a2e8a2:	8a2e                	mv	s4,a1
  a2e8a4:	0034                	addi	a3,sp,8
  a2e8a6:	4615                	li	a2,5
  a2e8a8:	85ce                	mv	a1,s3
  a2e8aa:	84aa                	mv	s1,a0
  a2e8ac:	03c020ef          	jal	ra,a308e8 <LzmaDec_AllocateProbs>
  a2e8b0:	c939                	beqz	a0,a2e906 <upg_lzma_init+0x90>
  a2e8b2:	842a                	mv	s0,a0
  a2e8b4:	b7dd                	j	a2e89a <upg_lzma_init+0x24>
  a2e8b6:	00f98733          	add	a4,s3,a5
  a2e8ba:	3358                	lbu	a4,5(a4)
  a2e8bc:	00379613          	slli	a2,a5,0x3
  a2e8c0:	0785                	addi	a5,a5,1
  a2e8c2:	00c71733          	sll	a4,a4,a2
  a2e8c6:	993a                	add	s2,s2,a4
  a2e8c8:	fed7e7e3          	bltu	a5,a3,a2e8b6 <upg_lzma_init+0x40>
  a2e8cc:	40c0                	lw	s0,4(s1)
  a2e8ce:	00897363          	bgeu	s2,s0,a2e8d4 <upg_lzma_init+0x5e>
  a2e8d2:	844a                	mv	s0,s2
  a2e8d4:	d061                	beqz	s0,a2e894 <upg_lzma_init+0x1e>
  a2e8d6:	85a2                	mv	a1,s0
  a2e8d8:	0028                	addi	a0,sp,8
  a2e8da:	f39ff0ef          	jal	ra,a2e812 <upg_lzma_alloc>
  a2e8de:	c888                	sw	a0,16(s1)
  a2e8e0:	cd19                	beqz	a0,a2e8fe <upg_lzma_init+0x88>
  a2e8e2:	c8c0                	sw	s0,20(s1)
  a2e8e4:	012a2823          	sw	s2,16(s4)
  a2e8e8:	8552                	mv	a0,s4
  a2e8ea:	f51ff0ef          	jal	ra,a2e83a <upg_lzma_init_buf>
  a2e8ee:	842a                	mv	s0,a0
  a2e8f0:	c105                	beqz	a0,a2e910 <upg_lzma_init+0x9a>
  a2e8f2:	488c                	lw	a1,16(s1)
  a2e8f4:	0028                	addi	a0,sp,8
  a2e8f6:	f3bff0ef          	jal	ra,a2e830 <upg_lzma_free>
  a2e8fa:	0004a823          	sw	zero,16(s1)
  a2e8fe:	80000437          	lui	s0,0x80000
  a2e902:	0415                	addi	s0,s0,5 # 80000005 <_gp_+0x7f5cb849>
  a2e904:	bf59                	j	a2e89a <upg_lzma_init+0x24>
  a2e906:	4781                	li	a5,0
  a2e908:	4901                	li	s2,0
  a2e90a:	ffb40693          	addi	a3,s0,-5
  a2e90e:	bf6d                	j	a2e8c8 <upg_lzma_init+0x52>
  a2e910:	8526                	mv	a0,s1
  a2e912:	351010ef          	jal	ra,a30462 <LzmaDec_Init>
  a2e916:	b751                	j	a2e89a <upg_lzma_init+0x24>

00a2e918 <upg_lzma_deinit>:
  a2e918:	8138                	push	{ra,s0-s1},-32
  a2e91a:	842e                	mv	s0,a1
  a2e91c:	490c                	lw	a1,16(a0)
  a2e91e:	00a2 e812 079f      	l.li	a5,0xa2e812
  a2e924:	84aa                	mv	s1,a0
  a2e926:	c43e                	sw	a5,8(sp)
  a2e928:	0028                	addi	a0,sp,8
  a2e92a:	00a2 e830 079f      	l.li	a5,0xa2e830
  a2e930:	c63e                	sw	a5,12(sp)
  a2e932:	effff0ef          	jal	ra,a2e830 <upg_lzma_free>
  a2e936:	002c                	addi	a1,sp,8
  a2e938:	8526                	mv	a0,s1
  a2e93a:	0004a823          	sw	zero,16(s1)
  a2e93e:	6ff010ef          	jal	ra,a3083c <LzmaDec_FreeProbs>
  a2e942:	484c                	lw	a1,20(s0)
  a2e944:	0028                	addi	a0,sp,8
  a2e946:	eebff0ef          	jal	ra,a2e830 <upg_lzma_free>
  a2e94a:	4c0c                	lw	a1,24(s0)
  a2e94c:	00042a23          	sw	zero,20(s0)
  a2e950:	0028                	addi	a0,sp,8
  a2e952:	edfff0ef          	jal	ra,a2e830 <upg_lzma_free>
  a2e956:	00042c23          	sw	zero,24(s0)
  a2e95a:	8134                	popret	{ra,s0-s1},32

00a2e95c <upg_lzma_write_image>:
  a2e95c:	8028                	push	{ra,s0},-16
  a2e95e:	51f8                	lw	a4,100(a1)
  a2e960:	3c78 96e1 079f      	l.li	a5,0x3c7896e1
  a2e966:	02f70263          	beq	a4,a5,a2e98a <upg_lzma_write_image+0x2e>
  a2e96a:	842a                	mv	s0,a0
  a2e96c:	4114                	lw	a3,0(a0)
  a2e96e:	01c50613          	addi	a2,a0,28
  a2e972:	4d0c                	lw	a1,24(a0)
  a2e974:	4508                	lw	a0,8(a0)
  a2e976:	4701                	li	a4,0
  a2e978:	23e5                	jal	ra,a2ef60 <upg_write_new_image_data>
  a2e97a:	e11d                	bnez	a0,a2e9a0 <upg_lzma_write_image+0x44>
  a2e97c:	441c                	lw	a5,8(s0)
  a2e97e:	4c58                	lw	a4,28(s0)
  a2e980:	00042e23          	sw	zero,28(s0)
  a2e984:	97ba                	add	a5,a5,a4
  a2e986:	c41c                	sw	a5,8(s0)
  a2e988:	8024                	popret	{ra,s0},16
  a2e98a:	de9ff0ef          	jal	ra,a2e772 <upg_encry_and_write_pkt>
  a2e98e:	dd6d                	beqz	a0,a2e988 <upg_lzma_write_image+0x2c>
  a2e990:	85aa                	mv	a1,a0
  a2e992:	00a3 38d8 051f      	l.li	a0,0xa338d8
  a2e998:	eacfd0ef          	jal	ra,a2c044 <print_str>
  a2e99c:	557d                	li	a0,-1
  a2e99e:	b7ed                	j	a2e988 <upg_lzma_write_image+0x2c>
  a2e9a0:	4505                	li	a0,1
  a2e9a2:	b7dd                	j	a2e988 <upg_lzma_write_image+0x2c>

00a2e9a4 <upg_lzma_decode_to_midbuf>:
  a2e9a4:	81a8                	push	{ra,s0-s8},-64
  a2e9a6:	0205a983          	lw	s3,32(a1) # 4020 <ccause+0x305e>
  a2e9aa:	8aaa                	mv	s5,a0
  a2e9ac:	84ae                	mv	s1,a1
  a2e9ae:	8b32                	mv	s6,a2
  a2e9b0:	8bb6                	mv	s7,a3
  a2e9b2:	000103a3          	sb	zero,7(sp)
  a2e9b6:	4901                	li	s2,0
  a2e9b8:	6a11                	lui	s4,0x4
  a2e9ba:	6c05                	lui	s8,0x1
  a2e9bc:	01396463          	bltu	s2,s3,a2e9c4 <upg_lzma_decode_to_midbuf+0x20>
  a2e9c0:	4501                	li	a0,0
  a2e9c2:	a041                	j	a2ea42 <upg_lzma_decode_to_midbuf+0x9e>
  a2e9c4:	4898                	lw	a4,16(s1)
  a2e9c6:	412987b3          	sub	a5,s3,s2
  a2e9ca:	c43e                	sw	a5,8(sp)
  a2e9cc:	c652                	sw	s4,12(sp)
  a2e9ce:	4781                	li	a5,0
  a2e9d0:	01477463          	bgeu	a4,s4,a2e9d8 <upg_lzma_decode_to_midbuf+0x34>
  a2e9d4:	c63a                	sw	a4,12(sp)
  a2e9d6:	4785                	li	a5,1
  a2e9d8:	4c88                	lw	a0,24(s1)
  a2e9da:	48d4                	lw	a3,20(s1)
  a2e9dc:	4ccc                	lw	a1,28(s1)
  a2e9de:	00710813          	addi	a6,sp,7
  a2e9e2:	0038                	addi	a4,sp,8
  a2e9e4:	95aa                	add	a1,a1,a0
  a2e9e6:	96ca                	add	a3,a3,s2
  a2e9e8:	0070                	addi	a2,sp,12
  a2e9ea:	8556                	mv	a0,s5
  a2e9ec:	5bd010ef          	jal	ra,a307a8 <LzmaDec_DecodeToBuf>
  a2e9f0:	842a                	mv	s0,a0
  a2e9f2:	c909                	beqz	a0,a2ea04 <upg_lzma_decode_to_midbuf+0x60>
  a2e9f4:	85aa                	mv	a1,a0
  a2e9f6:	00a3 3884 051f      	l.li	a0,0xa33884
  a2e9fc:	e48fd0ef          	jal	ra,a2c044 <print_str>
  a2ea00:	8522                	mv	a0,s0
  a2ea02:	a081                	j	a2ea42 <upg_lzma_decode_to_midbuf+0x9e>
  a2ea04:	47a2                	lw	a5,8(sp)
  a2ea06:	4532                	lw	a0,12(sp)
  a2ea08:	993e                	add	s2,s2,a5
  a2ea0a:	489c                	lw	a5,16(s1)
  a2ea0c:	8f89                	sub	a5,a5,a0
  a2ea0e:	c89c                	sw	a5,16(s1)
  a2ea10:	4cdc                	lw	a5,28(s1)
  a2ea12:	97aa                	add	a5,a5,a0
  a2ea14:	ccdc                	sw	a5,28(s1)
  a2ea16:	2321                	jal	ra,a2ef1e <upg_calculate_and_notify_process>
  a2ea18:	4cdc                	lw	a5,28(s1)
  a2ea1a:	40fa07b3          	sub	a5,s4,a5
  a2ea1e:	0187e663          	bltu	a5,s8,a2ea2a <upg_lzma_decode_to_midbuf+0x86>
  a2ea22:	489c                	lw	a5,16(s1)
  a2ea24:	c399                	beqz	a5,a2ea2a <upg_lzma_decode_to_midbuf+0x86>
  a2ea26:	01299f63          	bne	s3,s2,a2ea44 <upg_lzma_decode_to_midbuf+0xa0>
  a2ea2a:	865e                	mv	a2,s7
  a2ea2c:	85da                	mv	a1,s6
  a2ea2e:	8526                	mv	a0,s1
  a2ea30:	f2dff0ef          	jal	ra,a2e95c <upg_lzma_write_image>
  a2ea34:	c901                	beqz	a0,a2ea44 <upg_lzma_decode_to_midbuf+0xa0>
  a2ea36:	00a3 38b4 051f      	l.li	a0,0xa338b4
  a2ea3c:	e08fd0ef          	jal	ra,a2c044 <print_str>
  a2ea40:	557d                	li	a0,-1
  a2ea42:	81a4                	popret	{ra,s0-s8},64
  a2ea44:	4722                	lw	a4,8(sp)
  a2ea46:	489c                	lw	a5,16(s1)
  a2ea48:	e319                	bnez	a4,a2ea4e <upg_lzma_decode_to_midbuf+0xaa>
  a2ea4a:	4732                	lw	a4,12(sp)
  a2ea4c:	c319                	beqz	a4,a2ea52 <upg_lzma_decode_to_midbuf+0xae>
  a2ea4e:	dbcd                	beqz	a5,a2ea00 <upg_lzma_decode_to_midbuf+0x5c>
  a2ea50:	b7b5                	j	a2e9bc <upg_lzma_decode_to_midbuf+0x18>
  a2ea52:	d7dd                	beqz	a5,a2ea00 <upg_lzma_decode_to_midbuf+0x5c>
  a2ea54:	00714403          	lbu	s0,7(sp)
  a2ea58:	147d                	addi	s0,s0,-1
  a2ea5a:	00803433          	snez	s0,s0
  a2ea5e:	b74d                	j	a2ea00 <upg_lzma_decode_to_midbuf+0x5c>

00a2ea60 <upg_lzma_decode>:
  a2ea60:	8168                	push	{ra,s0-s4},-48
  a2ea62:	4785                	li	a5,1
  a2ea64:	89aa                	mv	s3,a0
  a2ea66:	84ae                	mv	s1,a1
  a2ea68:	8932                	mv	s2,a2
  a2ea6a:	00f105a3          	sb	a5,11(sp)
  a2ea6e:	6a05                	lui	s4,0x1
  a2ea70:	44c0                	lw	s0,12(s1)
  a2ea72:	c015                	beqz	s0,a2ea96 <upg_lzma_decode+0x36>
  a2ea74:	008a7363          	bgeu	s4,s0,a2ea7a <upg_lzma_decode+0x1a>
  a2ea78:	6405                	lui	s0,0x1
  a2ea7a:	48cc                	lw	a1,20(s1)
  a2ea7c:	40c8                	lw	a0,4(s1)
  a2ea7e:	0070                	addi	a2,sp,12
  a2ea80:	c622                	sw	s0,12(sp)
  a2ea82:	aacff0ef          	jal	ra,a2dd2e <upg_read_fota_pkg_data>
  a2ea86:	c911                	beqz	a0,a2ea9a <upg_lzma_decode+0x3a>
  a2ea88:	4405                	li	s0,1
  a2ea8a:	85a2                	mv	a1,s0
  a2ea8c:	00a3 3820 051f      	l.li	a0,0xa33820
  a2ea92:	db2fd0ef          	jal	ra,a2c044 <print_str>
  a2ea96:	8522                	mv	a0,s0
  a2ea98:	8164                	popret	{ra,s0-s4},48
  a2ea9a:	45b2                	lw	a1,12(sp)
  a2ea9c:	48c8                	lw	a0,20(s1)
  a2ea9e:	864a                	mv	a2,s2
  a2eaa0:	c15fe0ef          	jal	ra,a2d6b4 <upg_decry_fota_pkt>
  a2eaa4:	e51d                	bnez	a0,a2ead2 <upg_lzma_decode+0x72>
  a2eaa6:	4732                	lw	a4,12(sp)
  a2eaa8:	40dc                	lw	a5,4(s1)
  a2eaaa:	00b10693          	addi	a3,sp,11
  a2eaae:	d098                	sw	a4,32(s1)
  a2eab0:	97ba                	add	a5,a5,a4
  a2eab2:	c0dc                	sw	a5,4(s1)
  a2eab4:	44dc                	lw	a5,12(s1)
  a2eab6:	864a                	mv	a2,s2
  a2eab8:	85a6                	mv	a1,s1
  a2eaba:	8f99                	sub	a5,a5,a4
  a2eabc:	c4dc                	sw	a5,12(s1)
  a2eabe:	854e                	mv	a0,s3
  a2eac0:	ee5ff0ef          	jal	ra,a2e9a4 <upg_lzma_decode_to_midbuf>
  a2eac4:	842a                	mv	s0,a0
  a2eac6:	c901                	beqz	a0,a2ead6 <upg_lzma_decode+0x76>
  a2eac8:	85aa                	mv	a1,a0
  a2eaca:	00a3 3854 051f      	l.li	a0,0xa33854
  a2ead0:	b7c9                	j	a2ea92 <upg_lzma_decode+0x32>
  a2ead2:	547d                	li	s0,-1
  a2ead4:	bf5d                	j	a2ea8a <upg_lzma_decode+0x2a>
  a2ead6:	8f5fe0ef          	jal	ra,a2d3ca <upg_watchdog_kick>
  a2eada:	bf59                	j	a2ea70 <upg_lzma_decode+0x10>

00a2eadc <upg_check_first_entry>:
  a2eadc:	8148                	push	{ra,s0-s2},-32
  a2eade:	00a3 1c1c 079f      	l.li	a5,0xa31c1c
  a2eae4:	239a                	lhu	a4,0(a5)
  a2eae6:	23bc                	lbu	a5,2(a5)
  a2eae8:	892a                	mv	s2,a0
  a2eaea:	00e11623          	sh	a4,12(sp)
  a2eaee:	00f10723          	sb	a5,14(sp)
  a2eaf2:	01050493          	addi	s1,a0,16
  a2eaf6:	4401                	li	s0,0
  a2eaf8:	00c92783          	lw	a5,12(s2)
  a2eafc:	00f46463          	bltu	s0,a5,a2eb04 <upg_check_first_entry+0x28>
  a2eb00:	4505                	li	a0,1
  a2eb02:	8144                	popret	{ra,s0-s2},32
  a2eb04:	8526                	mv	a0,s1
  a2eb06:	460d                	li	a2,3
  a2eb08:	006c                	addi	a1,sp,12
  a2eb0a:	9e4fa0ef          	jal	ra,a28cee <memcmp>
  a2eb0e:	048d                	addi	s1,s1,3
  a2eb10:	e119                	bnez	a0,a2eb16 <upg_check_first_entry+0x3a>
  a2eb12:	0405                	addi	s0,s0,1 # 1001 <ccause+0x3f>
  a2eb14:	b7d5                	j	a2eaf8 <upg_check_first_entry+0x1c>
  a2eb16:	4501                	li	a0,0
  a2eb18:	b7ed                	j	a2eb02 <upg_check_first_entry+0x26>

00a2eb1a <uapi_upg_start>:
  a2eb1a:	82d8                	push	{ra,s0-s11},-96
  a2eb1c:	c602                	sw	zero,12(sp)
  a2eb1e:	c802                	sw	zero,16(sp)
  a2eb20:	e94ff0ef          	jal	ra,a2e1b4 <upg_is_inited>
  a2eb24:	3e050063          	beqz	a0,a2ef04 <uapi_upg_start+0x3ea>
  a2eb28:	84aa                	mv	s1,a0
  a2eb2a:	0068                	addi	a0,sp,12
  a2eb2c:	95aff0ef          	jal	ra,a2dc86 <upg_alloc_and_get_upgrade_flag>
  a2eb30:	892a                	mv	s2,a0
  a2eb32:	4501                	li	a0,0
  a2eb34:	02091b63          	bnez	s2,a2eb6a <uapi_upg_start+0x50>
  a2eb38:	47b2                	lw	a5,12(sp)
  a2eb3a:	cb85                	beqz	a5,a2eb6a <uapi_upg_start+0x50>
  a2eb3c:	4394                	lw	a3,0(a5)
  a2eb3e:	55aa 55aa 071f      	l.li	a4,0x55aa55aa
  a2eb44:	00e69a63          	bne	a3,a4,a2eb58 <uapi_upg_start+0x3e>
  a2eb48:	57b4                	lw	a3,104(a5)
  a2eb4a:	aa55 aa55 071f      	l.li	a4,0xaa55aa55
  a2eb50:	00e69463          	bne	a3,a4,a2eb58 <uapi_upg_start+0x3e>
  a2eb54:	53fc                	lw	a5,100(a5)
  a2eb56:	e795                	bnez	a5,a2eb82 <uapi_upg_start+0x68>
  a2eb58:	00a3 3908 051f      	l.li	a0,0xa33908
  a2eb5e:	ce6fd0ef          	jal	ra,a2c044 <print_str>
  a2eb62:	4501                	li	a0,0
  a2eb64:	8000 3047 091f      	l.li	s2,0x80003047
  a2eb6a:	8626                	mv	a2,s1
  a2eb6c:	85ca                	mv	a1,s2
  a2eb6e:	d84ff0ef          	jal	ra,a2e0f2 <upg_set_complete_flag>
  a2eb72:	4532                	lw	a0,12(sp)
  a2eb74:	8daff0ef          	jal	ra,a2dc4e <upg_free>
  a2eb78:	4542                	lw	a0,16(sp)
  a2eb7a:	8d4ff0ef          	jal	ra,a2dc4e <upg_free>
  a2eb7e:	854a                	mv	a0,s2
  a2eb80:	82d4                	popret	{ra,s0-s11},96
  a2eb82:	0808                	addi	a0,sp,16
  a2eb84:	a66ff0ef          	jal	ra,a2ddea <upg_get_package_header>
  a2eb88:	892a                	mv	s2,a0
  a2eb8a:	e119                	bnez	a0,a2eb90 <uapi_upg_start+0x76>
  a2eb8c:	47c2                	lw	a5,16(sp)
  a2eb8e:	eb81                	bnez	a5,a2eb9e <uapi_upg_start+0x84>
  a2eb90:	00a3 3700 051f      	l.li	a0,0xa33700
  a2eb96:	caefd0ef          	jal	ra,a2c044 <print_str>
  a2eb9a:	4501                	li	a0,0
  a2eb9c:	b7f9                	j	a2eb6a <uapi_upg_start+0x50>
  a2eb9e:	4532                	lw	a0,12(sp)
  a2eba0:	f3dff0ef          	jal	ra,a2eadc <upg_check_first_entry>
  a2eba4:	842a                	mv	s0,a0
  a2eba6:	ed05                	bnez	a0,a2ebde <uapi_upg_start+0xc4>
  a2eba8:	8d6ff0ef          	jal	ra,a2dc7e <upg_get_ctx>
  a2ebac:	49c2                	lw	s3,16(sp)
  a2ebae:	cc02                	sw	zero,24(sp)
  a2ebb0:	00052e23          	sw	zero,28(a0)
  a2ebb4:	02052023          	sw	zero,32(a0)
  a2ebb8:	842a                	mv	s0,a0
  a2ebba:	082c                	addi	a1,sp,24
  a2ebbc:	854e                	mv	a0,s3
  a2ebbe:	4b32                	lw	s6,12(sp)
  a2ebc0:	a4cff0ef          	jal	ra,a2de0c <upg_get_pkg_image_hash_table>
  a2ebc4:	892a                	mv	s2,a0
  a2ebc6:	c11d                	beqz	a0,a2ebec <uapi_upg_start+0xd2>
  a2ebc8:	00a3 3460 051f      	l.li	a0,0xa33460
  a2ebce:	c76fd0ef          	jal	ra,a2c044 <print_str>
  a2ebd2:	04090563          	beqz	s2,a2ec1c <uapi_upg_start+0x102>
  a2ebd6:	4505                	li	a0,1
  a2ebd8:	cb0ff0ef          	jal	ra,a2e088 <upg_set_temporary_result>
  a2ebdc:	bf7d                	j	a2eb9a <uapi_upg_start+0x80>
  a2ebde:	4542                	lw	a0,16(sp)
  a2ebe0:	949ff0ef          	jal	ra,a2e528 <uapi_upg_verify_file>
  a2ebe4:	892a                	mv	s2,a0
  a2ebe6:	d169                	beqz	a0,a2eba8 <uapi_upg_start+0x8e>
  a2ebe8:	84a2                	mv	s1,s0
  a2ebea:	bf45                	j	a2eb9a <uapi_upg_start+0x80>
  a2ebec:	47e2                	lw	a5,24(sp)
  a2ebee:	dfe9                	beqz	a5,a2ebc8 <uapi_upg_start+0xae>
  a2ebf0:	1489aa83          	lw	s5,328(s3)
  a2ebf4:	4a01                	li	s4,0
  a2ebf6:	4981                	li	s3,0
  a2ebf8:	02c00b93          	li	s7,44
  a2ebfc:	cb9e 063c 0c1f      	l.li	s8,0xcb9e063c
  a2ec02:	4c8d                	li	s9,3
  a2ec04:	4562                	lw	a0,24(sp)
  a2ec06:	075a4863          	blt	s4,s5,a2ec76 <uapi_upg_start+0x15c>
  a2ec0a:	844ff0ef          	jal	ra,a2dc4e <upg_free>
  a2ec0e:	5010                	lw	a2,32(s0)
  a2ec10:	4c4c                	lw	a1,28(s0)
  a2ec12:	00a3 3928 051f      	l.li	a0,0xa33928
  a2ec18:	c2cfd0ef          	jal	ra,a2c044 <print_str>
  a2ec1c:	44c2                	lw	s1,16(sp)
  a2ec1e:	4432                	lw	s0,12(sp)
  a2ec20:	00a3 395c 051f      	l.li	a0,0xa3395c
  a2ec26:	1484aa83          	lw	s5,328(s1)
  a2ec2a:	ca02                	sw	zero,20(sp)
  a2ec2c:	85d6                	mv	a1,s5
  a2ec2e:	c16fd0ef          	jal	ra,a2c044 <print_str>
  a2ec32:	444c                	lw	a1,12(s0)
  a2ec34:	00a3 3980 051f      	l.li	a0,0xa33980
  a2ec3a:	c0afd0ef          	jal	ra,a2c044 <print_str>
  a2ec3e:	445c                	lw	a5,12(s0)
  a2ec40:	00fa8663          	beq	s5,a5,a2ec4c <uapi_upg_start+0x132>
  a2ec44:	fffa8713          	addi	a4,s5,-1
  a2ec48:	2ae79a63          	bne	a5,a4,a2eefc <uapi_upg_start+0x3e2>
  a2ec4c:	4755                	li	a4,21
  a2ec4e:	2ae7f763          	bgeu	a5,a4,a2eefc <uapi_upg_start+0x3e2>
  a2ec52:	084c                	addi	a1,sp,20
  a2ec54:	8526                	mv	a0,s1
  a2ec56:	9b6ff0ef          	jal	ra,a2de0c <upg_get_pkg_image_hash_table>
  a2ec5a:	892a                	mv	s2,a0
  a2ec5c:	e119                	bnez	a0,a2ec62 <uapi_upg_start+0x148>
  a2ec5e:	44d2                	lw	s1,20(sp)
  a2ec60:	eca5                	bnez	s1,a2ecd8 <uapi_upg_start+0x1be>
  a2ec62:	00a3 3460 051f      	l.li	a0,0xa33460
  a2ec68:	bdcfd0ef          	jal	ra,a2c044 <print_str>
  a2ec6c:	47c2                	lw	a5,16(sp)
  a2ec6e:	4481                	li	s1,0
  a2ec70:	1487a503          	lw	a0,328(a5)
  a2ec74:	bddd                	j	a2eb6a <uapi_upg_start+0x50>
  a2ec76:	037a0d33          	mul	s10,s4,s7
  a2ec7a:	956a                	add	a0,a0,s10
  a2ec7c:	411c                	lw	a5,0(a0)
  a2ec7e:	01879663          	bne	a5,s8,a2ec8a <uapi_upg_start+0x170>
  a2ec82:	19fd                	addi	s3,s3,-1
  a2ec84:	0a05                	addi	s4,s4,1 # 1001 <ccause+0x3f>
  a2ec86:	0985                	addi	s3,s3,1
  a2ec88:	bfb5                	j	a2ec04 <uapi_upg_start+0xea>
  a2ec8a:	086c                	addi	a1,sp,28
  a2ec8c:	ce02                	sw	zero,28(sp)
  a2ec8e:	9b2ff0ef          	jal	ra,a2de40 <upg_get_pkg_image_header>
  a2ec92:	892a                	mv	s2,a0
  a2ec94:	e119                	bnez	a0,a2ec9a <uapi_upg_start+0x180>
  a2ec96:	47f2                	lw	a5,28(sp)
  a2ec98:	eb91                	bnez	a5,a2ecac <uapi_upg_start+0x192>
  a2ec9a:	00a3 34b4 051f      	l.li	a0,0xa334b4
  a2eca0:	ba4fd0ef          	jal	ra,a2c044 <print_str>
  a2eca4:	4562                	lw	a0,24(sp)
  a2eca6:	fa9fe0ef          	jal	ra,a2dc4e <upg_free>
  a2ecaa:	b725                	j	a2ebd2 <uapi_upg_start+0xb8>
  a2ecac:	4bfc                	lw	a5,84(a5)
  a2ecae:	4c58                	lw	a4,28(s0)
  a2ecb0:	85ce                	mv	a1,s3
  a2ecb2:	855a                	mv	a0,s6
  a2ecb4:	973e                	add	a4,a4,a5
  a2ecb6:	cc58                	sw	a4,28(s0)
  a2ecb8:	47e2                	lw	a5,24(sp)
  a2ecba:	97ea                	add	a5,a5,s10
  a2ecbc:	4390                	lw	a2,0(a5)
  a2ecbe:	b4cff0ef          	jal	ra,a2e00a <upg_get_image_update_status>
  a2ecc2:	01951763          	bne	a0,s9,a2ecd0 <uapi_upg_start+0x1b6>
  a2ecc6:	4772                	lw	a4,28(sp)
  a2ecc8:	501c                	lw	a5,32(s0)
  a2ecca:	4b78                	lw	a4,84(a4)
  a2eccc:	97ba                	add	a5,a5,a4
  a2ecce:	d01c                	sw	a5,32(s0)
  a2ecd0:	4572                	lw	a0,28(sp)
  a2ecd2:	f7dfe0ef          	jal	ra,a2dc4e <upg_free>
  a2ecd6:	b77d                	j	a2ec84 <uapi_upg_start+0x16a>
  a2ecd8:	cc02                	sw	zero,24(sp)
  a2ecda:	4981                	li	s3,0
  a2ecdc:	4b01                	li	s6,0
  a2ecde:	cb9e 063c 0a1f      	l.li	s4,0xcb9e063c
  a2ece4:	80003bb7          	lui	s7,0x80003
  a2ece8:	00a34d37          	lui	s10,0xa34
  a2ecec:	00a34db7          	lui	s11,0xa34
  a2ecf0:	055b5963          	bge	s6,s5,a2ed42 <uapi_upg_start+0x228>
  a2ecf4:	4088                	lw	a0,0(s1)
  a2ecf6:	01451363          	bne	a0,s4,a2ecfc <uapi_upg_start+0x1e2>
  a2ecfa:	19fd                	addi	s3,s3,-1
  a2ecfc:	e1efe0ef          	jal	ra,a2d31a <upg_img_in_set>
  a2ed00:	cd19                	beqz	a0,a2ed1e <uapi_upg_start+0x204>
  a2ed02:	4090                	lw	a2,0(s1)
  a2ed04:	85ce                	mv	a1,s3
  a2ed06:	8522                	mv	a0,s0
  a2ed08:	b02ff0ef          	jal	ra,a2e00a <upg_get_image_update_status>
  a2ed0c:	8c2a                	mv	s8,a0
  a2ed0e:	030516bb          	bnei	a0,3,a2ed28 <uapi_upg_start+0x20e>
  a2ed12:	408c                	lw	a1,0(s1)
  a2ed14:	00a3 39a8 051f      	l.li	a0,0xa339a8
  a2ed1a:	b2afd0ef          	jal	ra,a2c044 <print_str>
  a2ed1e:	0b05                	addi	s6,s6,1
  a2ed20:	0985                	addi	s3,s3,1
  a2ed22:	02c48493          	addi	s1,s1,44
  a2ed26:	b7e9                	j	a2ecf0 <uapi_upg_start+0x1d6>
  a2ed28:	082c                	addi	a1,sp,24
  a2ed2a:	8526                	mv	a0,s1
  a2ed2c:	914ff0ef          	jal	ra,a2de40 <upg_get_pkg_image_header>
  a2ed30:	892a                	mv	s2,a0
  a2ed32:	e119                	bnez	a0,a2ed38 <uapi_upg_start+0x21e>
  a2ed34:	47e2                	lw	a5,24(sp)
  a2ed36:	e795                	bnez	a5,a2ed62 <uapi_upg_start+0x248>
  a2ed38:	00a3 39d8 051f      	l.li	a0,0xa339d8
  a2ed3e:	b06fd0ef          	jal	ra,a2c044 <print_str>
  a2ed42:	4562                	lw	a0,24(sp)
  a2ed44:	c119                	beqz	a0,a2ed4a <uapi_upg_start+0x230>
  a2ed46:	f09fe0ef          	jal	ra,a2dc4e <upg_free>
  a2ed4a:	00090863          	beqz	s2,a2ed5a <uapi_upg_start+0x240>
  a2ed4e:	85ca                	mv	a1,s2
  a2ed50:	00a3 3b90 051f      	l.li	a0,0xa33b90
  a2ed56:	aeefd0ef          	jal	ra,a2c044 <print_str>
  a2ed5a:	4552                	lw	a0,20(sp)
  a2ed5c:	ef3fe0ef          	jal	ra,a2dc4e <upg_free>
  a2ed60:	b731                	j	a2ec6c <uapi_upg_start+0x152>
  a2ed62:	43dc                	lw	a5,4(a5)
  a2ed64:	13478163          	beq	a5,s4,a2ee86 <uapi_upg_start+0x36c>
  a2ed68:	4601                	li	a2,0
  a2ed6a:	85ce                	mv	a1,s3
  a2ed6c:	8522                	mv	a0,s0
  a2ed6e:	9aaff0ef          	jal	ra,a2df18 <upg_set_firmware_update_status>
  a2ed72:	892a                	mv	s2,a0
  a2ed74:	c509                	beqz	a0,a2ed7e <uapi_upg_start+0x264>
  a2ed76:	00a3 3a00 051f      	l.li	a0,0xa33a00
  a2ed7c:	b7c9                	j	a2ed3e <uapi_upg_start+0x224>
  a2ed7e:	47e2                	lw	a5,24(sp)
  a2ed80:	a2cd0513          	addi	a0,s10,-1492 # a33a2c <g_efuse_cfg+0x1230>
  a2ed84:	43cc                	lw	a1,4(a5)
  a2ed86:	abefd0ef          	jal	ra,a2c044 <print_str>
  a2ed8a:	4962                	lw	s2,24(sp)
  a2ed8c:	040c1963          	bnez	s8,a2edde <uapi_upg_start+0x2c4>
  a2ed90:	af4ff0ef          	jal	ra,a2e084 <upg_flash_erase_metadata_pages>
  a2ed94:	8caa                	mv	s9,a0
  a2ed96:	c521                	beqz	a0,a2edde <uapi_upg_start+0x2c4>
  a2ed98:	00a3 3a58 051f      	l.li	a0,0xa33a58
  a2ed9e:	aa6fd0ef          	jal	ra,a2c044 <print_str>
  a2eda2:	4c05                	li	s8,1
  a2eda4:	85e2                	mv	a1,s8
  a2eda6:	aecd8513          	addi	a0,s11,-1300 # a33aec <g_efuse_cfg+0x12f0>
  a2edaa:	a9afd0ef          	jal	ra,a2c044 <print_str>
  a2edae:	8662                	mv	a2,s8
  a2edb0:	85ce                	mv	a1,s3
  a2edb2:	8522                	mv	a0,s0
  a2edb4:	964ff0ef          	jal	ra,a2df18 <upg_set_firmware_update_status>
  a2edb8:	892a                	mv	s2,a0
  a2edba:	000c8363          	beqz	s9,a2edc0 <uapi_upg_start+0x2a6>
  a2edbe:	8966                	mv	s2,s9
  a2edc0:	85ca                	mv	a1,s2
  a2edc2:	00a3 3b0c 051f      	l.li	a0,0xa33b0c
  a2edc8:	a7cfd0ef          	jal	ra,a2c044 <print_str>
  a2edcc:	f6091be3          	bnez	s2,a2ed42 <uapi_upg_start+0x228>
  a2edd0:	4562                	lw	a0,24(sp)
  a2edd2:	e7dfe0ef          	jal	ra,a2dc4e <upg_free>
  a2edd6:	cc02                	sw	zero,24(sp)
  a2edd8:	df2fe0ef          	jal	ra,a2d3ca <upg_watchdog_kick>
  a2eddc:	b789                	j	a2ed1e <uapi_upg_start+0x204>
  a2edde:	001c3693          	seqz	a3,s8
  a2ede2:	02000613          	li	a2,32
  a2ede6:	00c48593          	addi	a1,s1,12
  a2edea:	854a                	mv	a0,s2
  a2edec:	eb0ff0ef          	jal	ra,a2e49c <uapi_upg_verify_file_image>
  a2edf0:	8caa                	mv	s9,a0
  a2edf2:	4c05                	li	s8,1
  a2edf4:	f945                	bnez	a0,a2eda4 <uapi_upg_start+0x28a>
  a2edf6:	00492683          	lw	a3,4(s2)
  a2edfa:	cb9e1737          	lui	a4,0xcb9e1
  a2edfe:	82670613          	addi	a2,a4,-2010 # cb9e0826 <_gp_+0xcafac06a>
  a2ee02:	046b8c93          	addi	s9,s7,70 # 80003046 <_gp_+0x7f5ce88a>
  a2ee06:	f8c68fe3          	beq	a3,a2,a2eda4 <uapi_upg_start+0x28a>
  a2ee0a:	83270713          	addi	a4,a4,-1998
  a2ee0e:	f8e68be3          	beq	a3,a4,a2eda4 <uapi_upg_start+0x28a>
  a2ee12:	06092583          	lw	a1,96(s2)
  a2ee16:	00a3 3a88 051f      	l.li	a0,0xa33a88
  a2ee1c:	a28fd0ef          	jal	ra,a2c044 <print_str>
  a2ee20:	06092783          	lw	a5,96(s2)
  a2ee24:	3c78 96e1 071f      	l.li	a4,0x3c7896e1
  a2ee2a:	02e79363          	bne	a5,a4,a2ee50 <uapi_upg_start+0x336>
  a2ee2e:	00a3 3aac 051f      	l.li	a0,0xa33aac
  a2ee34:	a10fd0ef          	jal	ra,a2c044 <print_str>
  a2ee38:	854a                	mv	a0,s2
  a2ee3a:	f0efe0ef          	jal	ra,a2d548 <upg_erase_whole_image>
  a2ee3e:	8caa                	mv	s9,a0
  a2ee40:	f135                	bnez	a0,a2eda4 <uapi_upg_start+0x28a>
  a2ee42:	854a                	mv	a0,s2
  a2ee44:	2add                	jal	ra,a2f03a <uapi_upg_compress_image_update>
  a2ee46:	00153c13          	seqz	s8,a0
  a2ee4a:	8caa                	mv	s9,a0
  a2ee4c:	0c05                	addi	s8,s8,1 # 1001 <ccause+0x3f>
  a2ee4e:	bf99                	j	a2eda4 <uapi_upg_start+0x28a>
  a2ee50:	4449 4646 071f      	l.li	a4,0x44494646
  a2ee56:	00e79a63          	bne	a5,a4,a2ee6a <uapi_upg_start+0x350>
  a2ee5a:	00a3 3ac4 051f      	l.li	a0,0xa33ac4
  a2ee60:	9e4fd0ef          	jal	ra,a2c044 <print_str>
  a2ee64:	854a                	mv	a0,s2
  a2ee66:	2c3d                	jal	ra,a2f0a4 <uapi_upg_diff_image_update>
  a2ee68:	bff9                	j	a2ee46 <uapi_upg_start+0x32c>
  a2ee6a:	00a3 3ad8 051f      	l.li	a0,0xa33ad8
  a2ee70:	9d4fd0ef          	jal	ra,a2c044 <print_str>
  a2ee74:	854a                	mv	a0,s2
  a2ee76:	ed2fe0ef          	jal	ra,a2d548 <upg_erase_whole_image>
  a2ee7a:	8caa                	mv	s9,a0
  a2ee7c:	f20514e3          	bnez	a0,a2eda4 <uapi_upg_start+0x28a>
  a2ee80:	854a                	mv	a0,s2
  a2ee82:	242d                	jal	ra,a2f0ac <uapi_upg_full_image_update>
  a2ee84:	b7c9                	j	a2ee46 <uapi_upg_start+0x32c>
  a2ee86:	00a347b7          	lui	a5,0xa34
  a2ee8a:	b3c78513          	addi	a0,a5,-1220 # a33b3c <g_efuse_cfg+0x1340>
  a2ee8e:	85d2                	mv	a1,s4
  a2ee90:	9b4fd0ef          	jal	ra,a2c044 <print_str>
  a2ee94:	47e2                	lw	a5,24(sp)
  a2ee96:	00a346b7          	lui	a3,0xa34
  a2ee9a:	9046a903          	lw	s2,-1788(a3) # a33904 <g_efuse_cfg+0x1108>
  a2ee9e:	43d8                	lw	a4,4(a5)
  a2eea0:	05471663          	bne	a4,s4,a2eeec <uapi_upg_start+0x3d2>
  a2eea4:	4794                	lw	a3,8(a5)
  a2eea6:	40d8                	lw	a4,4(s1)
  a2eea8:	0868                	addi	a0,sp,28
  a2eeaa:	c874                	sw	a3,84(s0)
  a2eeac:	47dc                	lw	a5,12(a5)
  a2eeae:	07bd                	addi	a5,a5,15
  a2eeb0:	9bc1                	andi	a5,a5,-16
  a2eeb2:	cc3c                	sw	a5,88(s0)
  a2eeb4:	01070793          	addi	a5,a4,16
  a2eeb8:	cc7c                	sw	a5,92(s0)
  a2eeba:	02000793          	li	a5,32
  a2eebe:	d03c                	sw	a5,96(s0)
  a2eec0:	ce02                	sw	zero,28(sp)
  a2eec2:	c7efe0ef          	jal	ra,a2d340 <upg_get_upgrade_flag_flash_start_addr>
  a2eec6:	892a                	mv	s2,a0
  a2eec8:	e115                	bnez	a0,a2eeec <uapi_upg_start+0x3d2>
  a2eeca:	4572                	lw	a0,28(sp)
  a2eecc:	4681                	li	a3,0
  a2eece:	05440613          	addi	a2,s0,84
  a2eed2:	45c1                	li	a1,16
  a2eed4:	05450513          	addi	a0,a0,84
  a2eed8:	dc6fe0ef          	jal	ra,a2d49e <upg_flash_write>
  a2eedc:	892a                	mv	s2,a0
  a2eede:	e519                	bnez	a0,a2eeec <uapi_upg_start+0x3d2>
  a2eee0:	4601                	li	a2,0
  a2eee2:	85d2                	mv	a1,s4
  a2eee4:	8522                	mv	a0,s0
  a2eee6:	832ff0ef          	jal	ra,a2df18 <upg_set_firmware_update_status>
  a2eeea:	892a                	mv	s2,a0
  a2eeec:	00a347b7          	lui	a5,0xa34
  a2eef0:	85ca                	mv	a1,s2
  a2eef2:	b6478513          	addi	a0,a5,-1180 # a33b64 <g_efuse_cfg+0x1368>
  a2eef6:	94efd0ef          	jal	ra,a2c044 <print_str>
  a2eefa:	bdd9                	j	a2edd0 <uapi_upg_start+0x2b6>
  a2eefc:	8000 3043 091f      	l.li	s2,0x80003043
  a2ef02:	b3ad                	j	a2ec6c <uapi_upg_start+0x152>
  a2ef04:	4485                	li	s1,1
  a2ef06:	4501                	li	a0,0
  a2ef08:	8000 3040 091f      	l.li	s2,0x80003040
  a2ef0e:	b9b1                	j	a2eb6a <uapi_upg_start+0x50>

00a2ef10 <uapi_upg_register_progress_callback>:
  a2ef10:	8028                	push	{ra,s0},-16
  a2ef12:	842a                	mv	s0,a0
  a2ef14:	d6bfe0ef          	jal	ra,a2dc7e <upg_get_ctx>
  a2ef18:	cd00                	sw	s0,24(a0)
  a2ef1a:	4501                	li	a0,0
  a2ef1c:	8024                	popret	{ra,s0},16

00a2ef1e <upg_calculate_and_notify_process>:
  a2ef1e:	8038                	push	{ra,s0-s1},-16
  a2ef20:	842a                	mv	s0,a0
  a2ef22:	d5dfe0ef          	jal	ra,a2dc7e <upg_get_ctx>
  a2ef26:	84aa                	mv	s1,a0
  a2ef28:	d57fe0ef          	jal	ra,a2dc7e <upg_get_ctx>
  a2ef2c:	4d1c                	lw	a5,24(a0)
  a2ef2e:	cb85                	beqz	a5,a2ef5e <upg_calculate_and_notify_process+0x40>
  a2ef30:	5088                	lw	a0,32(s1)
  a2ef32:	9522                	add	a0,a0,s0
  a2ef34:	4cc0                	lw	s0,28(s1)
  a2ef36:	d088                	sw	a0,32(s1)
  a2ef38:	c419                	beqz	s0,a2ef46 <upg_calculate_and_notify_process+0x28>
  a2ef3a:	06400793          	li	a5,100
  a2ef3e:	02f50533          	mul	a0,a0,a5
  a2ef42:	02855433          	divu	s0,a0,s0
  a2ef46:	00a0 5b00 049f      	l.li	s1,0xa05b00
  a2ef4c:	409c                	lw	a5,0(s1)
  a2ef4e:	00878863          	beq	a5,s0,a2ef5e <upg_calculate_and_notify_process+0x40>
  a2ef52:	d2dfe0ef          	jal	ra,a2dc7e <upg_get_ctx>
  a2ef56:	4d1c                	lw	a5,24(a0)
  a2ef58:	8522                	mv	a0,s0
  a2ef5a:	9782                	jalr	a5
  a2ef5c:	c080                	sw	s0,0(s1)
  a2ef5e:	8034                	popret	{ra,s0-s1},16

00a2ef60 <upg_write_new_image_data>:
  a2ef60:	8248                	push	{ra,s0-s2},-48
  a2ef62:	ca02                	sw	zero,20(sp)
  a2ef64:	cc02                	sw	zero,24(sp)
  a2ef66:	ce02                	sw	zero,28(sp)
  a2ef68:	4b87 a52d 079f      	l.li	a5,0x4b87a52d
  a2ef6e:	842a                	mv	s0,a0
  a2ef70:	892e                	mv	s2,a1
  a2ef72:	84b2                	mv	s1,a2
  a2ef74:	02f69363          	bne	a3,a5,a2ef9a <upg_write_new_image_data+0x3a>
  a2ef78:	002007b7          	lui	a5,0x200
  a2ef7c:	cc3e                	sw	a5,24(sp)
  a2ef7e:	78000793          	li	a5,1920
  a2ef82:	ce3e                	sw	a5,28(sp)
  a2ef84:	01414783          	lbu	a5,20(sp)
  a2ef88:	e38d                	bnez	a5,a2efaa <upg_write_new_image_data+0x4a>
  a2ef8a:	4562                	lw	a0,24(sp)
  a2ef8c:	408c                	lw	a1,0(s1)
  a2ef8e:	86ba                	mv	a3,a4
  a2ef90:	864a                	mv	a2,s2
  a2ef92:	9522                	add	a0,a0,s0
  a2ef94:	d0afe0ef          	jal	ra,a2d49e <upg_flash_write>
  a2ef98:	a801                	j	a2efa8 <upg_write_new_image_data+0x48>
  a2ef9a:	8536                	mv	a0,a3
  a2ef9c:	084c                	addi	a1,sp,20
  a2ef9e:	c63a                	sw	a4,12(sp)
  a2efa0:	d49fe0ef          	jal	ra,a2dce8 <upg_get_image_info>
  a2efa4:	4732                	lw	a4,12(sp)
  a2efa6:	dd79                	beqz	a0,a2ef84 <upg_write_new_image_data+0x24>
  a2efa8:	8244                	popret	{ra,s0-s2},48
  a2efaa:	8000 3004 051f      	l.li	a0,0x80003004
  a2efb0:	bfe5                	j	a2efa8 <upg_write_new_image_data+0x48>

00a2efb2 <uapi_upg_lzma_ota_image>:
  a2efb2:	8a58                	push	{ra,s0-s3},-192
  a2efb4:	842e                	mv	s0,a1
  a2efb6:	89b2                	mv	s3,a2
  a2efb8:	4581                	li	a1,0
  a2efba:	02400613          	li	a2,36
  a2efbe:	84aa                	mv	s1,a0
  a2efc0:	0828                	addi	a0,sp,24
  a2efc2:	8936                	mv	s2,a3
  a2efc4:	c63a                	sw	a4,12(sp)
  a2efc6:	9dff90ef          	jal	ra,a289a4 <memset>
  a2efca:	8622                	mv	a2,s0
  a2efcc:	86ce                	mv	a3,s3
  a2efce:	082c                	addi	a1,sp,24
  a2efd0:	1868                	addi	a0,sp,60
  a2efd2:	8a5ff0ef          	jal	ra,a2e876 <upg_lzma_init>
  a2efd6:	842a                	mv	s0,a0
  a2efd8:	e915                	bnez	a0,a2f00c <uapi_upg_lzma_ota_image+0x5a>
  a2efda:	44d0                	lw	a2,12(s1)
  a2efdc:	40dc                	lw	a5,4(s1)
  a2efde:	4732                	lw	a4,12(sp)
  a2efe0:	41360633          	sub	a2,a2,s3
  a2efe4:	d232                	sw	a2,36(sp)
  a2efe6:	5622                	lw	a2,40(sp)
  a2efe8:	cc3e                	sw	a5,24(sp)
  a2efea:	ce3a                	sw	a4,28(sp)
  a2efec:	d002                	sw	zero,32(sp)
  a2efee:	02c97163          	bgeu	s2,a2,a2f010 <uapi_upg_lzma_ota_image+0x5e>
  a2eff2:	85ca                	mv	a1,s2
  a2eff4:	00a3 3bc8 051f      	l.li	a0,0xa33bc8
  a2effa:	84afd0ef          	jal	ra,a2c044 <print_str>
  a2effe:	082c                	addi	a1,sp,24
  a2f000:	1868                	addi	a0,sp,60
  a2f002:	917ff0ef          	jal	ra,a2e918 <upg_lzma_deinit>
  a2f006:	8000 3051 041f      	l.li	s0,0x80003051
  a2f00c:	8522                	mv	a0,s0
  a2f00e:	8a54                	popret	{ra,s0-s3},192
  a2f010:	8626                	mv	a2,s1
  a2f012:	082c                	addi	a1,sp,24
  a2f014:	1868                	addi	a0,sp,60
  a2f016:	a4bff0ef          	jal	ra,a2ea60 <upg_lzma_decode>
  a2f01a:	842a                	mv	s0,a0
  a2f01c:	c911                	beqz	a0,a2f030 <uapi_upg_lzma_ota_image+0x7e>
  a2f01e:	85aa                	mv	a1,a0
  a2f020:	00a3 3c10 051f      	l.li	a0,0xa33c10
  a2f026:	81efd0ef          	jal	ra,a2c044 <print_str>
  a2f02a:	8000 3062 041f      	l.li	s0,0x80003062
  a2f030:	082c                	addi	a1,sp,24
  a2f032:	1868                	addi	a0,sp,60
  a2f034:	8e5ff0ef          	jal	ra,a2e918 <upg_lzma_deinit>
  a2f038:	bfd1                	j	a2f00c <uapi_upg_lzma_ota_image+0x5a>

00a2f03a <uapi_upg_compress_image_update>:
  a2f03a:	8258                	push	{ra,s0-s3},-64
  a2f03c:	47c1                	li	a5,16
  a2f03e:	0814                	addi	a3,sp,16
  a2f040:	0050                	addi	a2,sp,4
  a2f042:	4581                	li	a1,0
  a2f044:	84aa                	mv	s1,a0
  a2f046:	00852903          	lw	s2,8(a0)
  a2f04a:	c802                	sw	zero,16(sp)
  a2f04c:	ca02                	sw	zero,20(sp)
  a2f04e:	cc02                	sw	zero,24(sp)
  a2f050:	ce02                	sw	zero,28(sp)
  a2f052:	c23e                	sw	a5,4(sp)
  a2f054:	c402                	sw	zero,8(sp)
  a2f056:	c602                	sw	zero,12(sp)
  a2f058:	e7dfe0ef          	jal	ra,a2ded4 <upg_copy_pkg_image_data>
  a2f05c:	842a                	mv	s0,a0
  a2f05e:	e129                	bnez	a0,a2f0a0 <uapi_upg_compress_image_update+0x66>
  a2f060:	8526                	mv	a0,s1
  a2f062:	cfcfe0ef          	jal	ra,a2d55e <upg_cryto_init>
  a2f066:	842a                	mv	s0,a0
  a2f068:	ed05                	bnez	a0,a2f0a0 <uapi_upg_compress_image_update+0x66>
  a2f06a:	4592                	lw	a1,4(sp)
  a2f06c:	8626                	mv	a2,s1
  a2f06e:	0808                	addi	a0,sp,16
  a2f070:	e44fe0ef          	jal	ra,a2d6b4 <upg_decry_fota_pkt>
  a2f074:	842a                	mv	s0,a0
  a2f076:	e115                	bnez	a0,a2f09a <uapi_upg_compress_image_update+0x60>
  a2f078:	40c8                	lw	a0,4(s1)
  a2f07a:	0070                	addi	a2,sp,12
  a2f07c:	002c                	addi	a1,sp,8
  a2f07e:	4992                	lw	s3,4(sp)
  a2f080:	d21fe0ef          	jal	ra,a2dda0 <upg_get_partition_info>
  a2f084:	842a                	mv	s0,a0
  a2f086:	e911                	bnez	a0,a2f09a <uapi_upg_compress_image_update+0x60>
  a2f088:	46b2                	lw	a3,12(sp)
  a2f08a:	01298733          	add	a4,s3,s2
  a2f08e:	4641                	li	a2,16
  a2f090:	080c                	addi	a1,sp,16
  a2f092:	8526                	mv	a0,s1
  a2f094:	f1fff0ef          	jal	ra,a2efb2 <uapi_upg_lzma_ota_image>
  a2f098:	842a                	mv	s0,a0
  a2f09a:	8526                	mv	a0,s1
  a2f09c:	dd0fe0ef          	jal	ra,a2d66c <upg_cryto_deinit>
  a2f0a0:	8522                	mv	a0,s0
  a2f0a2:	8254                	popret	{ra,s0-s3},64

00a2f0a4 <uapi_upg_diff_image_update>:
  a2f0a4:	8000 3046 051f      	l.li	a0,0x80003046
  a2f0aa:	8082                	ret

00a2f0ac <uapi_upg_full_image_update>:
  a2f0ac:	8188                	push	{ra,s0-s6},-48
  a2f0ae:	00852a83          	lw	s5,8(a0)
  a2f0b2:	8a2a                	mv	s4,a0
  a2f0b4:	4148                	lw	a0,4(a0)
  a2f0b6:	0030                	addi	a2,sp,8
  a2f0b8:	004c                	addi	a1,sp,4
  a2f0ba:	c202                	sw	zero,4(sp)
  a2f0bc:	c402                	sw	zero,8(sp)
  a2f0be:	ce3fe0ef          	jal	ra,a2dda0 <upg_get_partition_info>
  a2f0c2:	842a                	mv	s0,a0
  a2f0c4:	e921                	bnez	a0,a2f114 <uapi_upg_full_image_update+0x68>
  a2f0c6:	00ca2703          	lw	a4,12(s4)
  a2f0ca:	47a2                	lw	a5,8(sp)
  a2f0cc:	06e7e963          	bltu	a5,a4,a2f13e <uapi_upg_full_image_update+0x92>
  a2f0d0:	6505                	lui	a0,0x1
  a2f0d2:	c602                	sw	zero,12(sp)
  a2f0d4:	b61fe0ef          	jal	ra,a2dc34 <upg_malloc>
  a2f0d8:	84aa                	mv	s1,a0
  a2f0da:	c525                	beqz	a0,a2f142 <uapi_upg_full_image_update+0x96>
  a2f0dc:	4901                	li	s2,0
  a2f0de:	6b05                	lui	s6,0x1
  a2f0e0:	00ca2783          	lw	a5,12(s4)
  a2f0e4:	00f96663          	bltu	s2,a5,a2f0f0 <uapi_upg_full_image_update+0x44>
  a2f0e8:	8526                	mv	a0,s1
  a2f0ea:	b65fe0ef          	jal	ra,a2dc4e <upg_free>
  a2f0ee:	a01d                	j	a2f114 <uapi_upg_full_image_update+0x68>
  a2f0f0:	412787b3          	sub	a5,a5,s2
  a2f0f4:	00fb7363          	bgeu	s6,a5,a2f0fa <uapi_upg_full_image_update+0x4e>
  a2f0f8:	6785                	lui	a5,0x1
  a2f0fa:	0070                	addi	a2,sp,12
  a2f0fc:	85a6                	mv	a1,s1
  a2f0fe:	01590533          	add	a0,s2,s5
  a2f102:	c63e                	sw	a5,12(sp)
  a2f104:	c2bfe0ef          	jal	ra,a2dd2e <upg_read_fota_pkg_data>
  a2f108:	89aa                	mv	s3,a0
  a2f10a:	c519                	beqz	a0,a2f118 <uapi_upg_full_image_update+0x6c>
  a2f10c:	8526                	mv	a0,s1
  a2f10e:	b41fe0ef          	jal	ra,a2dc4e <upg_free>
  a2f112:	844e                	mv	s0,s3
  a2f114:	8522                	mv	a0,s0
  a2f116:	8184                	popret	{ra,s0-s6},48
  a2f118:	004a2683          	lw	a3,4(s4)
  a2f11c:	4701                	li	a4,0
  a2f11e:	0070                	addi	a2,sp,12
  a2f120:	85a6                	mv	a1,s1
  a2f122:	854a                	mv	a0,s2
  a2f124:	e3dff0ef          	jal	ra,a2ef60 <upg_write_new_image_data>
  a2f128:	89aa                	mv	s3,a0
  a2f12a:	f16d                	bnez	a0,a2f10c <uapi_upg_full_image_update+0x60>
  a2f12c:	47b2                	lw	a5,12(sp)
  a2f12e:	dff9                	beqz	a5,a2f10c <uapi_upg_full_image_update+0x60>
  a2f130:	993e                	add	s2,s2,a5
  a2f132:	a98fe0ef          	jal	ra,a2d3ca <upg_watchdog_kick>
  a2f136:	4532                	lw	a0,12(sp)
  a2f138:	de7ff0ef          	jal	ra,a2ef1e <upg_calculate_and_notify_process>
  a2f13c:	b755                	j	a2f0e0 <uapi_upg_full_image_update+0x34>
  a2f13e:	547d                	li	s0,-1
  a2f140:	bfd1                	j	a2f114 <uapi_upg_full_image_update+0x68>
  a2f142:	80000437          	lui	s0,0x80000
  a2f146:	0415                	addi	s0,s0,5 # 80000005 <_gp_+0x7f5cb849>
  a2f148:	b7f1                	j	a2f114 <uapi_upg_full_image_update+0x68>

00a2f14a <LzmaDec_TryDummy>:
  a2f14a:	2138                	lbu	a4,2(a0)
  a2f14c:	02852f83          	lw	t6,40(a0) # 1028 <ccause+0x66>
  a2f150:	5e7d                	li	t3,-1
  a2f152:	00ee1e33          	sll	t3,t3,a4
  a2f156:	04052e83          	lw	t4,64(a0)
  a2f15a:	fffe4e13          	not	t3,t3
  a2f15e:	00c52303          	lw	t1,12(a0)
  a2f162:	01fe7e33          	and	t3,t3,t6
  a2f166:	0e12                	slli	t3,t3,0x4
  a2f168:	02052803          	lw	a6,32(a0)
  a2f16c:	01ce8f33          	add	t5,t4,t3
  a2f170:	03e30f1b          	addshf	t5,t1,t5,sll,1
  a2f174:	86aa                	mv	a3,a0
  a2f176:	515c                	lw	a5,36(a0)
  a2f178:	e00f0f13          	addi	t5,t5,-512
  a2f17c:	01000537          	lui	a0,0x1000
  a2f180:	00062883          	lw	a7,0(a2)
  a2f184:	000f5703          	lhu	a4,0(t5)
  a2f188:	00a87a63          	bgeu	a6,a0,a2f19c <LzmaDec_TryDummy+0x52>
  a2f18c:	4501                	li	a0,0
  a2f18e:	4115f963          	bgeu	a1,a7,a2f5a0 <LzmaDec_TryDummy+0x456>
  a2f192:	2188                	lbu	a0,0(a1)
  a2f194:	0822                	slli	a6,a6,0x8
  a2f196:	0585                	addi	a1,a1,1
  a2f198:	10f5279b          	orshf	a5,a0,a5,sll,8
  a2f19c:	00b85513          	srli	a0,a6,0xb
  a2f1a0:	02e50733          	mul	a4,a0,a4
  a2f1a4:	12e7f963          	bgeu	a5,a4,a2f2d6 <LzmaDec_TryDummy+0x18c>
  a2f1a8:	56c8                	lw	a0,44(a3)
  a2f1aa:	28030313          	addi	t1,t1,640
  a2f1ae:	e119                	bnez	a0,a2f1b4 <LzmaDec_TryDummy+0x6a>
  a2f1b0:	040f8863          	beqz	t6,a2f200 <LzmaDec_TryDummy+0xb6>
  a2f1b4:	0016c803          	lbu	a6,1(a3)
  a2f1b8:	557d                	li	a0,-1
  a2f1ba:	0006c283          	lbu	t0,0(a3)
  a2f1be:	01051533          	sll	a0,a0,a6
  a2f1c2:	0186a803          	lw	a6,24(a3)
  a2f1c6:	fff54513          	not	a0,a0
  a2f1ca:	01f57533          	and	a0,a0,t6
  a2f1ce:	00551533          	sll	a0,a0,t0
  a2f1d2:	0106af03          	lw	t5,16(a3)
  a2f1d6:	fff80e13          	addi	t3,a6,-1
  a2f1da:	00081563          	bnez	a6,a2f1e4 <LzmaDec_TryDummy+0x9a>
  a2f1de:	0146ae03          	lw	t3,20(a3)
  a2f1e2:	1e7d                	addi	t3,t3,-1
  a2f1e4:	9e7a                	add	t3,t3,t5
  a2f1e6:	000e4803          	lbu	a6,0(t3)
  a2f1ea:	4e21                	li	t3,8
  a2f1ec:	405e0e33          	sub	t3,t3,t0
  a2f1f0:	01c85833          	srl	a6,a6,t3
  a2f1f4:	9542                	add	a0,a0,a6
  a2f1f6:	60000813          	li	a6,1536
  a2f1fa:	03050533          	mul	a0,a0,a6
  a2f1fe:	932a                	add	t1,t1,a0
  a2f200:	071ef63b          	bgeui	t4,7,a2f258 <LzmaDec_TryDummy+0x10e>
  a2f204:	4685                	li	a3,1
  a2f206:	01000e37          	lui	t3,0x1000
  a2f20a:	0ff00813          	li	a6,255
  a2f20e:	0686                	slli	a3,a3,0x1
  a2f210:	00d30533          	add	a0,t1,a3
  a2f214:	00055e83          	lhu	t4,0(a0) # 1000000 <_gp_+0x5cb844>
  a2f218:	01c77963          	bgeu	a4,t3,a2f22a <LzmaDec_TryDummy+0xe0>
  a2f21c:	3915f163          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f220:	2188                	lbu	a0,0(a1)
  a2f222:	0722                	slli	a4,a4,0x8
  a2f224:	0585                	addi	a1,a1,1
  a2f226:	10f5279b          	orshf	a5,a0,a5,sll,8
  a2f22a:	00b75513          	srli	a0,a4,0xb
  a2f22e:	03d50533          	mul	a0,a0,t4
  a2f232:	02a7e163          	bltu	a5,a0,a2f254 <LzmaDec_TryDummy+0x10a>
  a2f236:	8f09                	sub	a4,a4,a0
  a2f238:	8f89                	sub	a5,a5,a0
  a2f23a:	0685                	addi	a3,a3,1
  a2f23c:	fcd879e3          	bgeu	a6,a3,a2f20e <LzmaDec_TryDummy+0xc4>
  a2f240:	4505                	li	a0,1
  a2f242:	010007b7          	lui	a5,0x1000
  a2f246:	00f77563          	bgeu	a4,a5,a2f250 <LzmaDec_TryDummy+0x106>
  a2f24a:	3515fa63          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f24e:	0585                	addi	a1,a1,1
  a2f250:	c20c                	sw	a1,0(a2)
  a2f252:	8082                	ret
  a2f254:	872a                	mv	a4,a0
  a2f256:	b7dd                	j	a2f23c <LzmaDec_TryDummy+0xf2>
  a2f258:	0186ae83          	lw	t4,24(a3)
  a2f25c:	0306ae03          	lw	t3,48(a3)
  a2f260:	4a88                	lw	a0,16(a3)
  a2f262:	4801                	li	a6,0
  a2f264:	41ce8f33          	sub	t5,t4,t3
  a2f268:	01cef463          	bgeu	t4,t3,a2f270 <LzmaDec_TryDummy+0x126>
  a2f26c:	0146a803          	lw	a6,20(a3)
  a2f270:	01e506b3          	add	a3,a0,t5
  a2f274:	96c2                	add	a3,a3,a6
  a2f276:	0006ce03          	lbu	t3,0(a3)
  a2f27a:	10000813          	li	a6,256
  a2f27e:	4685                	li	a3,1
  a2f280:	01000fb7          	lui	t6,0x1000
  a2f284:	0ff00f13          	li	t5,255
  a2f288:	0e06                	slli	t3,t3,0x1
  a2f28a:	01c87eb3          	and	t4,a6,t3
  a2f28e:	00d80533          	add	a0,a6,a3
  a2f292:	9576                	add	a0,a0,t4
  a2f294:	02a3051b          	addshf	a0,t1,a0,sll,1
  a2f298:	00055283          	lhu	t0,0(a0)
  a2f29c:	01f77963          	bgeu	a4,t6,a2f2ae <LzmaDec_TryDummy+0x164>
  a2f2a0:	2f15ff63          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f2a4:	2188                	lbu	a0,0(a1)
  a2f2a6:	0722                	slli	a4,a4,0x8
  a2f2a8:	0585                	addi	a1,a1,1
  a2f2aa:	10f5279b          	orshf	a5,a0,a5,sll,8
  a2f2ae:	00b75513          	srli	a0,a4,0xb
  a2f2b2:	02550533          	mul	a0,a0,t0
  a2f2b6:	0686                	slli	a3,a3,0x1
  a2f2b8:	00a7fa63          	bgeu	a5,a0,a2f2cc <LzmaDec_TryDummy+0x182>
  a2f2bc:	fffe4713          	not	a4,t3
  a2f2c0:	00e87833          	and	a6,a6,a4
  a2f2c4:	872a                	mv	a4,a0
  a2f2c6:	fcdf71e3          	bgeu	t5,a3,a2f288 <LzmaDec_TryDummy+0x13e>
  a2f2ca:	bf9d                	j	a2f240 <LzmaDec_TryDummy+0xf6>
  a2f2cc:	8f09                	sub	a4,a4,a0
  a2f2ce:	8f89                	sub	a5,a5,a0
  a2f2d0:	0685                	addi	a3,a3,1
  a2f2d2:	8876                	mv	a6,t4
  a2f2d4:	bfcd                	j	a2f2c6 <LzmaDec_TryDummy+0x17c>
  a2f2d6:	0ec1                	addi	t4,t4,16
  a2f2d8:	40e80833          	sub	a6,a6,a4
  a2f2dc:	03d30e9b          	addshf	t4,t1,t4,sll,1
  a2f2e0:	010006b7          	lui	a3,0x1000
  a2f2e4:	8f99                	sub	a5,a5,a4
  a2f2e6:	000ed703          	lhu	a4,0(t4)
  a2f2ea:	00d87a63          	bgeu	a6,a3,a2f2fe <LzmaDec_TryDummy+0x1b4>
  a2f2ee:	4501                	li	a0,0
  a2f2f0:	2b15f863          	bgeu	a1,a7,a2f5a0 <LzmaDec_TryDummy+0x456>
  a2f2f4:	2194                	lbu	a3,0(a1)
  a2f2f6:	0822                	slli	a6,a6,0x8
  a2f2f8:	0585                	addi	a1,a1,1
  a2f2fa:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2f2fe:	00b85693          	srli	a3,a6,0xb
  a2f302:	02e686b3          	mul	a3,a3,a4
  a2f306:	14d7f063          	bgeu	a5,a3,a2f446 <LzmaDec_TryDummy+0x2fc>
  a2f30a:	a0030813          	addi	a6,t1,-1536
  a2f30e:	4509                	li	a0,2
  a2f310:	4f81                	li	t6,0
  a2f312:	01000eb7          	lui	t4,0x1000
  a2f316:	00085703          	lhu	a4,0(a6)
  a2f31a:	01d6fa63          	bgeu	a3,t4,a2f32e <LzmaDec_TryDummy+0x1e4>
  a2f31e:	2915f063          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f322:	0005ce83          	lbu	t4,0(a1)
  a2f326:	06a2                	slli	a3,a3,0x8
  a2f328:	0585                	addi	a1,a1,1
  a2f32a:	10fea79b          	orshf	a5,t4,a5,sll,8
  a2f32e:	00b6de93          	srli	t4,a3,0xb
  a2f332:	02ee8733          	mul	a4,t4,a4
  a2f336:	1ce7fe63          	bgeu	a5,a4,a2f512 <LzmaDec_TryDummy+0x3c8>
  a2f33a:	03c8081b          	addshf	a6,a6,t3,sll,1
  a2f33e:	4681                	li	a3,0
  a2f340:	4f21                	li	t5,8
  a2f342:	4e05                	li	t3,1
  a2f344:	010002b7          	lui	t0,0x1000
  a2f348:	0e06                	slli	t3,t3,0x1
  a2f34a:	01c80eb3          	add	t4,a6,t3
  a2f34e:	000ed383          	lhu	t2,0(t4) # 1000000 <_gp_+0x5cb844>
  a2f352:	00577a63          	bgeu	a4,t0,a2f366 <LzmaDec_TryDummy+0x21c>
  a2f356:	2515f463          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f35a:	0005ce83          	lbu	t4,0(a1)
  a2f35e:	0722                	slli	a4,a4,0x8
  a2f360:	0585                	addi	a1,a1,1
  a2f362:	10fea79b          	orshf	a5,t4,a5,sll,8
  a2f366:	00b75e93          	srli	t4,a4,0xb
  a2f36a:	027e8eb3          	mul	t4,t4,t2
  a2f36e:	1fd7e663          	bltu	a5,t4,a2f55a <LzmaDec_TryDummy+0x410>
  a2f372:	41d70733          	sub	a4,a4,t4
  a2f376:	41d787b3          	sub	a5,a5,t4
  a2f37a:	0e05                	addi	t3,t3,1 # 1000001 <_gp_+0x5cb845>
  a2f37c:	fdee66e3          	bltu	t3,t5,a2f348 <LzmaDec_TryDummy+0x1fe>
  a2f380:	04bff0bb          	bgeui	t6,4,a2f242 <LzmaDec_TryDummy+0xf8>
  a2f384:	41e686b3          	sub	a3,a3,t5
  a2f388:	96f2                	add	a3,a3,t3
  a2f38a:	20000e13          	li	t3,512
  a2f38e:	0306f2bb          	bgeui	a3,3,a2f398 <LzmaDec_TryDummy+0x24e>
  a2f392:	0685                	addi	a3,a3,1 # 1000001 <_gp_+0x5cb845>
  a2f394:	00769e13          	slli	t3,a3,0x7
  a2f398:	4685                	li	a3,1
  a2f39a:	01000f37          	lui	t5,0x1000
  a2f39e:	03f00e93          	li	t4,63
  a2f3a2:	0686                	slli	a3,a3,0x1
  a2f3a4:	00d30833          	add	a6,t1,a3
  a2f3a8:	9872                	add	a6,a6,t3
  a2f3aa:	00085f83          	lhu	t6,0(a6)
  a2f3ae:	01e77a63          	bgeu	a4,t5,a2f3c2 <LzmaDec_TryDummy+0x278>
  a2f3b2:	1f15f663          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f3b6:	0005c803          	lbu	a6,0(a1)
  a2f3ba:	0722                	slli	a4,a4,0x8
  a2f3bc:	0585                	addi	a1,a1,1
  a2f3be:	10f8279b          	orshf	a5,a6,a5,sll,8
  a2f3c2:	00b75813          	srli	a6,a4,0xb
  a2f3c6:	03f80833          	mul	a6,a6,t6
  a2f3ca:	1907ea63          	bltu	a5,a6,a2f55e <LzmaDec_TryDummy+0x414>
  a2f3ce:	41070733          	sub	a4,a4,a6
  a2f3d2:	410787b3          	sub	a5,a5,a6
  a2f3d6:	0685                	addi	a3,a3,1
  a2f3d8:	fcdef5e3          	bgeu	t4,a3,a2f3a2 <LzmaDec_TryDummy+0x258>
  a2f3dc:	fc068693          	addi	a3,a3,-64
  a2f3e0:	0496e8bb          	bltui	a3,4,a2f242 <LzmaDec_TryDummy+0xf8>
  a2f3e4:	0016d813          	srli	a6,a3,0x1
  a2f3e8:	0e56febb          	bgeui	a3,14,a2f562 <LzmaDec_TryDummy+0x418>
  a2f3ec:	8a85                	andi	a3,a3,1
  a2f3ee:	187d                	addi	a6,a6,-1
  a2f3f0:	0026e693          	ori	a3,a3,2
  a2f3f4:	80000e37          	lui	t3,0x80000
  a2f3f8:	010696b3          	sll	a3,a3,a6
  a2f3fc:	980e4e13          	xori	t3,t3,-1664
  a2f400:	96f2                	add	a3,a3,t3
  a2f402:	02d3031b          	addshf	t1,t1,a3,sll,1
  a2f406:	4e85                	li	t4,1
  a2f408:	4e05                	li	t3,1
  a2f40a:	01000fb7          	lui	t6,0x1000
  a2f40e:	03c3069b          	addshf	a3,t1,t3,sll,1
  a2f412:	0006df03          	lhu	t5,0(a3)
  a2f416:	01f77963          	bgeu	a4,t6,a2f428 <LzmaDec_TryDummy+0x2de>
  a2f41a:	1915f263          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f41e:	2194                	lbu	a3,0(a1)
  a2f420:	0722                	slli	a4,a4,0x8
  a2f422:	0585                	addi	a1,a1,1
  a2f424:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2f428:	00b75693          	srli	a3,a4,0xb
  a2f42c:	03e686b3          	mul	a3,a3,t5
  a2f430:	001e9f13          	slli	t5,t4,0x1
  a2f434:	14d7ff63          	bgeu	a5,a3,a2f592 <LzmaDec_TryDummy+0x448>
  a2f438:	9e76                	add	t3,t3,t4
  a2f43a:	8736                	mv	a4,a3
  a2f43c:	187d                	addi	a6,a6,-1
  a2f43e:	8efa                	mv	t4,t5
  a2f440:	fc0817e3          	bnez	a6,a2f40e <LzmaDec_TryDummy+0x2c4>
  a2f444:	bbfd                	j	a2f242 <LzmaDec_TryDummy+0xf8>
  a2f446:	40d80733          	sub	a4,a6,a3
  a2f44a:	8f95                	sub	a5,a5,a3
  a2f44c:	010006b7          	lui	a3,0x1000
  a2f450:	018ed803          	lhu	a6,24(t4)
  a2f454:	00d77a63          	bgeu	a4,a3,a2f468 <LzmaDec_TryDummy+0x31e>
  a2f458:	4501                	li	a0,0
  a2f45a:	1515f363          	bgeu	a1,a7,a2f5a0 <LzmaDec_TryDummy+0x456>
  a2f45e:	2194                	lbu	a3,0(a1)
  a2f460:	0722                	slli	a4,a4,0x8
  a2f462:	0585                	addi	a1,a1,1
  a2f464:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2f468:	00b75693          	srli	a3,a4,0xb
  a2f46c:	030686b3          	mul	a3,a3,a6
  a2f470:	04d7f163          	bgeu	a5,a3,a2f4b2 <LzmaDec_TryDummy+0x368>
  a2f474:	777d                	lui	a4,0xfffff
  a2f476:	9f3a                	add	t5,t5,a4
  a2f478:	01000537          	lui	a0,0x1000
  a2f47c:	600f5703          	lhu	a4,1536(t5) # 1000600 <_gp_+0x5cbe44>
  a2f480:	00a6fa63          	bgeu	a3,a0,a2f494 <LzmaDec_TryDummy+0x34a>
  a2f484:	4501                	li	a0,0
  a2f486:	1115fd63          	bgeu	a1,a7,a2f5a0 <LzmaDec_TryDummy+0x456>
  a2f48a:	2188                	lbu	a0,0(a1)
  a2f48c:	06a2                	slli	a3,a3,0x8
  a2f48e:	0585                	addi	a1,a1,1
  a2f490:	10f5279b          	orshf	a5,a0,a5,sll,8
  a2f494:	00b6d513          	srli	a0,a3,0xb
  a2f498:	02e50733          	mul	a4,a0,a4
  a2f49c:	0ee7ef63          	bltu	a5,a4,a2f59a <LzmaDec_TryDummy+0x450>
  a2f4a0:	8e99                	sub	a3,a3,a4
  a2f4a2:	8f99                	sub	a5,a5,a4
  a2f4a4:	ffff f600 081f      	l.li	a6,0xfffff600
  a2f4aa:	981a                	add	a6,a6,t1
  a2f4ac:	450d                	li	a0,3
  a2f4ae:	4fb1                	li	t6,12
  a2f4b0:	b58d                	j	a2f312 <LzmaDec_TryDummy+0x1c8>
  a2f4b2:	8f15                	sub	a4,a4,a3
  a2f4b4:	8f95                	sub	a5,a5,a3
  a2f4b6:	010006b7          	lui	a3,0x1000
  a2f4ba:	030ed803          	lhu	a6,48(t4)
  a2f4be:	00d77a63          	bgeu	a4,a3,a2f4d2 <LzmaDec_TryDummy+0x388>
  a2f4c2:	4501                	li	a0,0
  a2f4c4:	0d15fe63          	bgeu	a1,a7,a2f5a0 <LzmaDec_TryDummy+0x456>
  a2f4c8:	2194                	lbu	a3,0(a1)
  a2f4ca:	0722                	slli	a4,a4,0x8
  a2f4cc:	0585                	addi	a1,a1,1
  a2f4ce:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2f4d2:	00b75693          	srli	a3,a4,0xb
  a2f4d6:	030686b3          	mul	a3,a3,a6
  a2f4da:	fcd7e5e3          	bltu	a5,a3,a2f4a4 <LzmaDec_TryDummy+0x35a>
  a2f4de:	8f15                	sub	a4,a4,a3
  a2f4e0:	8f95                	sub	a5,a5,a3
  a2f4e2:	010006b7          	lui	a3,0x1000
  a2f4e6:	048ed803          	lhu	a6,72(t4)
  a2f4ea:	00d77a63          	bgeu	a4,a3,a2f4fe <LzmaDec_TryDummy+0x3b4>
  a2f4ee:	4501                	li	a0,0
  a2f4f0:	0b15f863          	bgeu	a1,a7,a2f5a0 <LzmaDec_TryDummy+0x456>
  a2f4f4:	2194                	lbu	a3,0(a1)
  a2f4f6:	0722                	slli	a4,a4,0x8
  a2f4f8:	0585                	addi	a1,a1,1
  a2f4fa:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2f4fe:	00b75693          	srli	a3,a4,0xb
  a2f502:	030686b3          	mul	a3,a3,a6
  a2f506:	f8d7efe3          	bltu	a5,a3,a2f4a4 <LzmaDec_TryDummy+0x35a>
  a2f50a:	8f95                	sub	a5,a5,a3
  a2f50c:	40d706b3          	sub	a3,a4,a3
  a2f510:	bf51                	j	a2f4a4 <LzmaDec_TryDummy+0x35a>
  a2f512:	8e99                	sub	a3,a3,a4
  a2f514:	01000eb7          	lui	t4,0x1000
  a2f518:	8f99                	sub	a5,a5,a4
  a2f51a:	01085703          	lhu	a4,16(a6)
  a2f51e:	01d6fa63          	bgeu	a3,t4,a2f532 <LzmaDec_TryDummy+0x3e8>
  a2f522:	0715fe63          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f526:	0005ce83          	lbu	t4,0(a1)
  a2f52a:	06a2                	slli	a3,a3,0x8
  a2f52c:	0585                	addi	a1,a1,1
  a2f52e:	10fea79b          	orshf	a5,t4,a5,sll,8
  a2f532:	00b6de93          	srli	t4,a3,0xb
  a2f536:	02ee8733          	mul	a4,t4,a4
  a2f53a:	00e7f763          	bgeu	a5,a4,a2f548 <LzmaDec_TryDummy+0x3fe>
  a2f53e:	0e21                	addi	t3,t3,8 # 80000008 <_gp_+0x7f5cb84c>
  a2f540:	03c8081b          	addshf	a6,a6,t3,sll,1
  a2f544:	46a1                	li	a3,8
  a2f546:	bbed                	j	a2f340 <LzmaDec_TryDummy+0x1f6>
  a2f548:	8f99                	sub	a5,a5,a4
  a2f54a:	20080813          	addi	a6,a6,512
  a2f54e:	40e68733          	sub	a4,a3,a4
  a2f552:	10000f13          	li	t5,256
  a2f556:	46c1                	li	a3,16
  a2f558:	b3ed                	j	a2f342 <LzmaDec_TryDummy+0x1f8>
  a2f55a:	8776                	mv	a4,t4
  a2f55c:	b505                	j	a2f37c <LzmaDec_TryDummy+0x232>
  a2f55e:	8742                	mv	a4,a6
  a2f560:	bda5                	j	a2f3d8 <LzmaDec_TryDummy+0x28e>
  a2f562:	186d                	addi	a6,a6,-5
  a2f564:	01000e37          	lui	t3,0x1000
  a2f568:	01c77963          	bgeu	a4,t3,a2f57a <LzmaDec_TryDummy+0x430>
  a2f56c:	0315f963          	bgeu	a1,a7,a2f59e <LzmaDec_TryDummy+0x454>
  a2f570:	2194                	lbu	a3,0(a1)
  a2f572:	0722                	slli	a4,a4,0x8
  a2f574:	0585                	addi	a1,a1,1
  a2f576:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2f57a:	8305                	srli	a4,a4,0x1
  a2f57c:	40e786b3          	sub	a3,a5,a4
  a2f580:	82fd                	srli	a3,a3,0x1f
  a2f582:	16fd                	addi	a3,a3,-1 # ffffff <_gp_+0x5cb843>
  a2f584:	8ef9                	and	a3,a3,a4
  a2f586:	187d                	addi	a6,a6,-1
  a2f588:	8f95                	sub	a5,a5,a3
  a2f58a:	fc081fe3          	bnez	a6,a2f568 <LzmaDec_TryDummy+0x41e>
  a2f58e:	4811                	li	a6,4
  a2f590:	bd9d                	j	a2f406 <LzmaDec_TryDummy+0x2bc>
  a2f592:	8f15                	sub	a4,a4,a3
  a2f594:	8f95                	sub	a5,a5,a3
  a2f596:	9e7a                	add	t3,t3,t5
  a2f598:	b555                	j	a2f43c <LzmaDec_TryDummy+0x2f2>
  a2f59a:	450d                	li	a0,3
  a2f59c:	b15d                	j	a2f242 <LzmaDec_TryDummy+0xf8>
  a2f59e:	4501                	li	a0,0
  a2f5a0:	8082                	ret

00a2f5a2 <LzmaDec_DecodeReal2>:
  a2f5a2:	711d                	addi	sp,sp,-96
  a2f5a4:	03010293          	addi	t0,sp,48
  a2f5a8:	7fe2960b          	stmia	{s0-s11},(t0)
  a2f5ac:	02c52d83          	lw	s11,44(a0) # 100002c <_gp_+0x5cb870>
  a2f5b0:	ce32                	sw	a2,28(sp)
  a2f5b2:	8e2a                	mv	t3,a0
  a2f5b4:	02852383          	lw	t2,40(a0)
  a2f5b8:	01852e83          	lw	t4,24(a0)
  a2f5bc:	000d9b63          	bnez	s11,a2f5d2 <LzmaDec_DecodeReal2+0x30>
  a2f5c0:	415c                	lw	a5,4(a0)
  a2f5c2:	41d58733          	sub	a4,a1,t4
  a2f5c6:	407787b3          	sub	a5,a5,t2
  a2f5ca:	00e7f463          	bgeu	a5,a4,a2f5d2 <LzmaDec_DecodeReal2+0x30>
  a2f5ce:	01d785b3          	add	a1,a5,t4
  a2f5d2:	000e4783          	lbu	a5,0(t3) # 1000000 <_gp_+0x5cb844>
  a2f5d6:	001e4703          	lbu	a4,1(t3)
  a2f5da:	002e4603          	lbu	a2,2(t3)
  a2f5de:	c63e                	sw	a5,12(sp)
  a2f5e0:	46b2                	lw	a3,12(sp)
  a2f5e2:	10000793          	li	a5,256
  a2f5e6:	00e79733          	sll	a4,a5,a4
  a2f5ea:	00d7d7b3          	srl	a5,a5,a3
  a2f5ee:	56fd                	li	a3,-1
  a2f5f0:	00c696b3          	sll	a3,a3,a2
  a2f5f4:	fff6c693          	not	a3,a3
  a2f5f8:	cc36                	sw	a3,24(sp)
  a2f5fa:	800006b7          	lui	a3,0x80000
  a2f5fe:	00ce2f83          	lw	t6,12(t3)
  a2f602:	f006c693          	xori	a3,a3,-256
  a2f606:	d036                	sw	a3,32(sp)
  a2f608:	76fd                	lui	a3,0xfffff
  a2f60a:	60068693          	addi	a3,a3,1536 # fffff600 <_gp_+0xff5cae44>
  a2f60e:	014e2c83          	lw	s9,20(t3)
  a2f612:	40f707b3          	sub	a5,a4,a5
  a2f616:	c836                	sw	a3,16(sp)
  a2f618:	96fe                	add	a3,a3,t6
  a2f61a:	ca3e                	sw	a5,20(sp)
  a2f61c:	d236                	sw	a3,36(sp)
  a2f61e:	040e2f03          	lw	t5,64(t3)
  a2f622:	a00f8693          	addi	a3,t6,-1536 # fffa00 <_gp_+0x5cb244>
  a2f626:	030e2283          	lw	t0,48(t3)
  a2f62a:	034e2403          	lw	s0,52(t3)
  a2f62e:	038e2983          	lw	s3,56(t3)
  a2f632:	03ce2d03          	lw	s10,60(t3)
  a2f636:	010e2a03          	lw	s4,16(t3)
  a2f63a:	01ce2703          	lw	a4,28(t3)
  a2f63e:	020e2903          	lw	s2,32(t3)
  a2f642:	024e2783          	lw	a5,36(t3)
  a2f646:	d436                	sw	a3,40(sp)
  a2f648:	fffc8693          	addi	a3,s9,-1
  a2f64c:	4881                	li	a7,0
  a2f64e:	01000637          	lui	a2,0x1000
  a2f652:	0000 0800 081f      	l.li	a6,0x800
  a2f658:	d636                	sw	a3,44(sp)
  a2f65a:	46e2                	lw	a3,24(sp)
  a2f65c:	0076f533          	and	a0,a3,t2
  a2f660:	5682                	lw	a3,32(sp)
  a2f662:	0512                	slli	a0,a0,0x4
  a2f664:	01e504b3          	add	s1,a0,t5
  a2f668:	94b6                	add	s1,s1,a3
  a2f66a:	0486                	slli	s1,s1,0x1
  a2f66c:	009f8ab3          	add	s5,t6,s1
  a2f670:	000ad303          	lhu	t1,0(s5)
  a2f674:	00c97763          	bgeu	s2,a2,a2f682 <LzmaDec_DecodeReal2+0xe0>
  a2f678:	2314                	lbu	a3,0(a4)
  a2f67a:	0922                	slli	s2,s2,0x8
  a2f67c:	0705                	addi	a4,a4,1 # fffff001 <_gp_+0xff5ca845>
  a2f67e:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2f682:	00b95693          	srli	a3,s2,0xb
  a2f686:	026686b3          	mul	a3,a3,t1
  a2f68a:	54d7fb63          	bgeu	a5,a3,a2fbe0 <LzmaDec_DecodeReal2+0x63e>
  a2f68e:	40680533          	sub	a0,a6,t1
  a2f692:	4aa3031b          	addshf	t1,t1,a0,srl,5
  a2f696:	006a9023          	sh	t1,0(s5)
  a2f69a:	280f8513          	addi	a0,t6,640
  a2f69e:	00039463          	bnez	t2,a2f6a6 <LzmaDec_DecodeReal2+0x104>
  a2f6a2:	020d8563          	beqz	s11,a2f6cc <LzmaDec_DecodeReal2+0x12a>
  a2f6a6:	00839493          	slli	s1,t2,0x8
  a2f6aa:	fffe8313          	addi	t1,t4,-1 # ffffff <_gp_+0x5cb843>
  a2f6ae:	000e9363          	bnez	t4,a2f6b4 <LzmaDec_DecodeReal2+0x112>
  a2f6b2:	5332                	lw	t1,44(sp)
  a2f6b4:	9352                	add	t1,t1,s4
  a2f6b6:	00034303          	lbu	t1,0(t1)
  a2f6ba:	9326                	add	t1,t1,s1
  a2f6bc:	44d2                	lw	s1,20(sp)
  a2f6be:	00937333          	and	t1,t1,s1
  a2f6c2:	44b2                	lw	s1,12(sp)
  a2f6c4:	00931333          	sll	t1,t1,s1
  a2f6c8:	0665155b          	muliadd	a0,a0,t1,6
  a2f6cc:	0385                	addi	t2,t2,1
  a2f6ce:	431d                	li	t1,7
  a2f6d0:	246f7163          	bgeu	t5,t1,a2f912 <LzmaDec_DecodeReal2+0x370>
  a2f6d4:	448d                	li	s1,3
  a2f6d6:	837a                	mv	t1,t5
  a2f6d8:	01e4f363          	bgeu	s1,t5,a2f6de <LzmaDec_DecodeReal2+0x13c>
  a2f6dc:	430d                	li	t1,3
  a2f6de:	406f0f33          	sub	t5,t5,t1
  a2f6e2:	00255303          	lhu	t1,2(a0)
  a2f6e6:	00c6f763          	bgeu	a3,a2,a2f6f4 <LzmaDec_DecodeReal2+0x152>
  a2f6ea:	2304                	lbu	s1,0(a4)
  a2f6ec:	06a2                	slli	a3,a3,0x8
  a2f6ee:	0705                	addi	a4,a4,1
  a2f6f0:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a2f6f4:	00b6d493          	srli	s1,a3,0xb
  a2f6f8:	026484b3          	mul	s1,s1,t1
  a2f6fc:	1697ff63          	bgeu	a5,s1,a2f87a <LzmaDec_DecodeReal2+0x2d8>
  a2f700:	406806b3          	sub	a3,a6,t1
  a2f704:	4ad3031b          	addshf	t1,t1,a3,srl,5
  a2f708:	00651123          	sh	t1,2(a0)
  a2f70c:	4689                	li	a3,2
  a2f70e:	0686                	slli	a3,a3,0x1
  a2f710:	00d50ab3          	add	s5,a0,a3
  a2f714:	000ad903          	lhu	s2,0(s5)
  a2f718:	00c4f863          	bgeu	s1,a2,a2f728 <LzmaDec_DecodeReal2+0x186>
  a2f71c:	00074303          	lbu	t1,0(a4)
  a2f720:	04a2                	slli	s1,s1,0x8
  a2f722:	0705                	addi	a4,a4,1
  a2f724:	10f3279b          	orshf	a5,t1,a5,sll,8
  a2f728:	00b4d313          	srli	t1,s1,0xb
  a2f72c:	03230333          	mul	t1,t1,s2
  a2f730:	1467fe63          	bgeu	a5,t1,a2f88c <LzmaDec_DecodeReal2+0x2ea>
  a2f734:	412804b3          	sub	s1,a6,s2
  a2f738:	4a99091b          	addshf	s2,s2,s1,srl,5
  a2f73c:	012a9023          	sh	s2,0(s5)
  a2f740:	0686                	slli	a3,a3,0x1
  a2f742:	00d50ab3          	add	s5,a0,a3
  a2f746:	000ad903          	lhu	s2,0(s5)
  a2f74a:	00c37763          	bgeu	t1,a2,a2f758 <LzmaDec_DecodeReal2+0x1b6>
  a2f74e:	2304                	lbu	s1,0(a4)
  a2f750:	0322                	slli	t1,t1,0x8
  a2f752:	0705                	addi	a4,a4,1
  a2f754:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a2f758:	00b35493          	srli	s1,t1,0xb
  a2f75c:	032484b3          	mul	s1,s1,s2
  a2f760:	1497f063          	bgeu	a5,s1,a2f8a0 <LzmaDec_DecodeReal2+0x2fe>
  a2f764:	41280333          	sub	t1,a6,s2
  a2f768:	4a69091b          	addshf	s2,s2,t1,srl,5
  a2f76c:	012a9023          	sh	s2,0(s5)
  a2f770:	0686                	slli	a3,a3,0x1
  a2f772:	00d50ab3          	add	s5,a0,a3
  a2f776:	000ad903          	lhu	s2,0(s5)
  a2f77a:	00c4f863          	bgeu	s1,a2,a2f78a <LzmaDec_DecodeReal2+0x1e8>
  a2f77e:	00074303          	lbu	t1,0(a4)
  a2f782:	04a2                	slli	s1,s1,0x8
  a2f784:	0705                	addi	a4,a4,1
  a2f786:	10f3279b          	orshf	a5,t1,a5,sll,8
  a2f78a:	00b4d313          	srli	t1,s1,0xb
  a2f78e:	03230333          	mul	t1,t1,s2
  a2f792:	1267f063          	bgeu	a5,t1,a2f8b2 <LzmaDec_DecodeReal2+0x310>
  a2f796:	412804b3          	sub	s1,a6,s2
  a2f79a:	4a99091b          	addshf	s2,s2,s1,srl,5
  a2f79e:	012a9023          	sh	s2,0(s5)
  a2f7a2:	0686                	slli	a3,a3,0x1
  a2f7a4:	00d50ab3          	add	s5,a0,a3
  a2f7a8:	000ad903          	lhu	s2,0(s5)
  a2f7ac:	00c37763          	bgeu	t1,a2,a2f7ba <LzmaDec_DecodeReal2+0x218>
  a2f7b0:	2304                	lbu	s1,0(a4)
  a2f7b2:	0322                	slli	t1,t1,0x8
  a2f7b4:	0705                	addi	a4,a4,1
  a2f7b6:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a2f7ba:	00b35493          	srli	s1,t1,0xb
  a2f7be:	032484b3          	mul	s1,s1,s2
  a2f7c2:	1097f263          	bgeu	a5,s1,a2f8c6 <LzmaDec_DecodeReal2+0x324>
  a2f7c6:	41280333          	sub	t1,a6,s2
  a2f7ca:	4a69091b          	addshf	s2,s2,t1,srl,5
  a2f7ce:	012a9023          	sh	s2,0(s5)
  a2f7d2:	0686                	slli	a3,a3,0x1
  a2f7d4:	00d50ab3          	add	s5,a0,a3
  a2f7d8:	000ad903          	lhu	s2,0(s5)
  a2f7dc:	00c4f863          	bgeu	s1,a2,a2f7ec <LzmaDec_DecodeReal2+0x24a>
  a2f7e0:	00074303          	lbu	t1,0(a4)
  a2f7e4:	04a2                	slli	s1,s1,0x8
  a2f7e6:	0705                	addi	a4,a4,1
  a2f7e8:	10f3279b          	orshf	a5,t1,a5,sll,8
  a2f7ec:	00b4d313          	srli	t1,s1,0xb
  a2f7f0:	03230333          	mul	t1,t1,s2
  a2f7f4:	0e67f263          	bgeu	a5,t1,a2f8d8 <LzmaDec_DecodeReal2+0x336>
  a2f7f8:	412804b3          	sub	s1,a6,s2
  a2f7fc:	4a99091b          	addshf	s2,s2,s1,srl,5
  a2f800:	012a9023          	sh	s2,0(s5)
  a2f804:	0686                	slli	a3,a3,0x1
  a2f806:	00d50ab3          	add	s5,a0,a3
  a2f80a:	000ad903          	lhu	s2,0(s5)
  a2f80e:	00c37763          	bgeu	t1,a2,a2f81c <LzmaDec_DecodeReal2+0x27a>
  a2f812:	2304                	lbu	s1,0(a4)
  a2f814:	0322                	slli	t1,t1,0x8
  a2f816:	0705                	addi	a4,a4,1
  a2f818:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a2f81c:	00b35493          	srli	s1,t1,0xb
  a2f820:	032484b3          	mul	s1,s1,s2
  a2f824:	0c97f463          	bgeu	a5,s1,a2f8ec <LzmaDec_DecodeReal2+0x34a>
  a2f828:	41280333          	sub	t1,a6,s2
  a2f82c:	4a69091b          	addshf	s2,s2,t1,srl,5
  a2f830:	012a9023          	sh	s2,0(s5)
  a2f834:	0686                	slli	a3,a3,0x1
  a2f836:	9536                	add	a0,a0,a3
  a2f838:	00055a83          	lhu	s5,0(a0)
  a2f83c:	00c4f863          	bgeu	s1,a2,a2f84c <LzmaDec_DecodeReal2+0x2aa>
  a2f840:	00074303          	lbu	t1,0(a4)
  a2f844:	04a2                	slli	s1,s1,0x8
  a2f846:	0705                	addi	a4,a4,1
  a2f848:	10f3279b          	orshf	a5,t1,a5,sll,8
  a2f84c:	00b4d913          	srli	s2,s1,0xb
  a2f850:	03590933          	mul	s2,s2,s5
  a2f854:	0b27f563          	bgeu	a5,s2,a2f8fe <LzmaDec_DecodeReal2+0x35c>
  a2f858:	41580333          	sub	t1,a6,s5
  a2f85c:	4a6a8a9b          	addshf	s5,s5,t1,srl,5
  a2f860:	01551023          	sh	s5,0(a0)
  a2f864:	01da0533          	add	a0,s4,t4
  a2f868:	a114                	sb	a3,0(a0)
  a2f86a:	0e85                	addi	t4,t4,1
  a2f86c:	30bef9e3          	bgeu	t4,a1,a3037e <LzmaDec_DecodeReal2+0xddc>
  a2f870:	46f2                	lw	a3,28(sp)
  a2f872:	ded764e3          	bltu	a4,a3,a2f65a <LzmaDec_DecodeReal2+0xb8>
  a2f876:	3090006f          	j	a3037e <LzmaDec_DecodeReal2+0xddc>
  a2f87a:	4a63131b          	subshf	t1,t1,t1,srl,5
  a2f87e:	8f85                	sub	a5,a5,s1
  a2f880:	00651123          	sh	t1,2(a0)
  a2f884:	409684b3          	sub	s1,a3,s1
  a2f888:	468d                	li	a3,3
  a2f88a:	b551                	j	a2f70e <LzmaDec_DecodeReal2+0x16c>
  a2f88c:	4b29191b          	subshf	s2,s2,s2,srl,5
  a2f890:	406787b3          	sub	a5,a5,t1
  a2f894:	012a9023          	sh	s2,0(s5)
  a2f898:	0685                	addi	a3,a3,1
  a2f89a:	40648333          	sub	t1,s1,t1
  a2f89e:	b54d                	j	a2f740 <LzmaDec_DecodeReal2+0x19e>
  a2f8a0:	4b29191b          	subshf	s2,s2,s2,srl,5
  a2f8a4:	8f85                	sub	a5,a5,s1
  a2f8a6:	012a9023          	sh	s2,0(s5)
  a2f8aa:	0685                	addi	a3,a3,1
  a2f8ac:	409304b3          	sub	s1,t1,s1
  a2f8b0:	b5c1                	j	a2f770 <LzmaDec_DecodeReal2+0x1ce>
  a2f8b2:	4b29191b          	subshf	s2,s2,s2,srl,5
  a2f8b6:	406787b3          	sub	a5,a5,t1
  a2f8ba:	012a9023          	sh	s2,0(s5)
  a2f8be:	0685                	addi	a3,a3,1
  a2f8c0:	40648333          	sub	t1,s1,t1
  a2f8c4:	bdf9                	j	a2f7a2 <LzmaDec_DecodeReal2+0x200>
  a2f8c6:	4b29191b          	subshf	s2,s2,s2,srl,5
  a2f8ca:	8f85                	sub	a5,a5,s1
  a2f8cc:	012a9023          	sh	s2,0(s5)
  a2f8d0:	0685                	addi	a3,a3,1
  a2f8d2:	409304b3          	sub	s1,t1,s1
  a2f8d6:	bdf5                	j	a2f7d2 <LzmaDec_DecodeReal2+0x230>
  a2f8d8:	4b29191b          	subshf	s2,s2,s2,srl,5
  a2f8dc:	406787b3          	sub	a5,a5,t1
  a2f8e0:	012a9023          	sh	s2,0(s5)
  a2f8e4:	0685                	addi	a3,a3,1
  a2f8e6:	40648333          	sub	t1,s1,t1
  a2f8ea:	bf29                	j	a2f804 <LzmaDec_DecodeReal2+0x262>
  a2f8ec:	4b29191b          	subshf	s2,s2,s2,srl,5
  a2f8f0:	8f85                	sub	a5,a5,s1
  a2f8f2:	012a9023          	sh	s2,0(s5)
  a2f8f6:	0685                	addi	a3,a3,1
  a2f8f8:	409304b3          	sub	s1,t1,s1
  a2f8fc:	bf25                	j	a2f834 <LzmaDec_DecodeReal2+0x292>
  a2f8fe:	4b5a9a9b          	subshf	s5,s5,s5,srl,5
  a2f902:	412787b3          	sub	a5,a5,s2
  a2f906:	01551023          	sh	s5,0(a0)
  a2f90a:	0685                	addi	a3,a3,1
  a2f90c:	41248933          	sub	s2,s1,s2
  a2f910:	bf91                	j	a2f864 <LzmaDec_DecodeReal2+0x2c2>
  a2f912:	405e8333          	sub	t1,t4,t0
  a2f916:	4481                	li	s1,0
  a2f918:	005ef363          	bgeu	t4,t0,a2f91e <LzmaDec_DecodeReal2+0x37c>
  a2f91c:	84e6                	mv	s1,s9
  a2f91e:	9352                	add	t1,t1,s4
  a2f920:	9326                	add	t1,t1,s1
  a2f922:	00034303          	lbu	t1,0(t1)
  a2f926:	4499                	li	s1,6
  a2f928:	0a0f71bb          	bgeui	t5,10,a2f92e <LzmaDec_DecodeReal2+0x38c>
  a2f92c:	448d                	li	s1,3
  a2f92e:	0306                	slli	t1,t1,0x1
  a2f930:	10037b93          	andi	s7,t1,256
  a2f934:	101b8a93          	addi	s5,s7,257
  a2f938:	03550a9b          	addshf	s5,a0,s5,sll,1
  a2f93c:	409f0f33          	sub	t5,t5,s1
  a2f940:	000ad903          	lhu	s2,0(s5)
  a2f944:	00c6f763          	bgeu	a3,a2,a2f952 <LzmaDec_DecodeReal2+0x3b0>
  a2f948:	2304                	lbu	s1,0(a4)
  a2f94a:	06a2                	slli	a3,a3,0x8
  a2f94c:	0705                	addi	a4,a4,1
  a2f94e:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a2f952:	00b6d493          	srli	s1,a3,0xb
  a2f956:	032484b3          	mul	s1,s1,s2
  a2f95a:	1e97fa63          	bgeu	a5,s1,a2fb4e <LzmaDec_DecodeReal2+0x5ac>
  a2f95e:	412806b3          	sub	a3,a6,s2
  a2f962:	4ad9091b          	addshf	s2,s2,a3,srl,5
  a2f966:	fff34b93          	not	s7,t1
  a2f96a:	012a9023          	sh	s2,0(s5)
  a2f96e:	100bfb93          	andi	s7,s7,256
  a2f972:	4689                	li	a3,2
  a2f974:	0306                	slli	t1,t1,0x1
  a2f976:	01737c33          	and	s8,t1,s7
  a2f97a:	00db8ab3          	add	s5,s7,a3
  a2f97e:	9ae2                	add	s5,s5,s8
  a2f980:	03550a9b          	addshf	s5,a0,s5,sll,1
  a2f984:	000adb03          	lhu	s6,0(s5)
  a2f988:	00c4f863          	bgeu	s1,a2,a2f998 <LzmaDec_DecodeReal2+0x3f6>
  a2f98c:	00074903          	lbu	s2,0(a4)
  a2f990:	04a2                	slli	s1,s1,0x8
  a2f992:	0705                	addi	a4,a4,1
  a2f994:	10f9279b          	orshf	a5,s2,a5,sll,8
  a2f998:	00b4d913          	srli	s2,s1,0xb
  a2f99c:	03690933          	mul	s2,s2,s6
  a2f9a0:	0686                	slli	a3,a3,0x1
  a2f9a2:	1b27ff63          	bgeu	a5,s2,a2fb60 <LzmaDec_DecodeReal2+0x5be>
  a2f9a6:	416804b3          	sub	s1,a6,s6
  a2f9aa:	4a9b0b1b          	addshf	s6,s6,s1,srl,5
  a2f9ae:	fff34c13          	not	s8,t1
  a2f9b2:	016a9023          	sh	s6,0(s5)
  a2f9b6:	017c7c33          	and	s8,s8,s7
  a2f9ba:	0306                	slli	t1,t1,0x1
  a2f9bc:	01837bb3          	and	s7,t1,s8
  a2f9c0:	00dc0ab3          	add	s5,s8,a3
  a2f9c4:	9ade                	add	s5,s5,s7
  a2f9c6:	03550a9b          	addshf	s5,a0,s5,sll,1
  a2f9ca:	000adb03          	lhu	s6,0(s5)
  a2f9ce:	00c97763          	bgeu	s2,a2,a2f9dc <LzmaDec_DecodeReal2+0x43a>
  a2f9d2:	2304                	lbu	s1,0(a4)
  a2f9d4:	0922                	slli	s2,s2,0x8
  a2f9d6:	0705                	addi	a4,a4,1
  a2f9d8:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a2f9dc:	00b95493          	srli	s1,s2,0xb
  a2f9e0:	036484b3          	mul	s1,s1,s6
  a2f9e4:	0686                	slli	a3,a3,0x1
  a2f9e6:	1897f763          	bgeu	a5,s1,a2fb74 <LzmaDec_DecodeReal2+0x5d2>
  a2f9ea:	41680933          	sub	s2,a6,s6
  a2f9ee:	4b2b0b1b          	addshf	s6,s6,s2,srl,5
  a2f9f2:	fff34b93          	not	s7,t1
  a2f9f6:	016a9023          	sh	s6,0(s5)
  a2f9fa:	018bfbb3          	and	s7,s7,s8
  a2f9fe:	0306                	slli	t1,t1,0x1
  a2fa00:	01737c33          	and	s8,t1,s7
  a2fa04:	00db8ab3          	add	s5,s7,a3
  a2fa08:	9ae2                	add	s5,s5,s8
  a2fa0a:	03550a9b          	addshf	s5,a0,s5,sll,1
  a2fa0e:	000adb03          	lhu	s6,0(s5)
  a2fa12:	00c4f863          	bgeu	s1,a2,a2fa22 <LzmaDec_DecodeReal2+0x480>
  a2fa16:	00074903          	lbu	s2,0(a4)
  a2fa1a:	04a2                	slli	s1,s1,0x8
  a2fa1c:	0705                	addi	a4,a4,1
  a2fa1e:	10f9279b          	orshf	a5,s2,a5,sll,8
  a2fa22:	00b4d913          	srli	s2,s1,0xb
  a2fa26:	03690933          	mul	s2,s2,s6
  a2fa2a:	0686                	slli	a3,a3,0x1
  a2fa2c:	1527fd63          	bgeu	a5,s2,a2fb86 <LzmaDec_DecodeReal2+0x5e4>
  a2fa30:	416804b3          	sub	s1,a6,s6
  a2fa34:	4a9b0b1b          	addshf	s6,s6,s1,srl,5
  a2fa38:	fff34c13          	not	s8,t1
  a2fa3c:	016a9023          	sh	s6,0(s5)
  a2fa40:	017c7c33          	and	s8,s8,s7
  a2fa44:	0306                	slli	t1,t1,0x1
  a2fa46:	01837bb3          	and	s7,t1,s8
  a2fa4a:	00dc0ab3          	add	s5,s8,a3
  a2fa4e:	9ade                	add	s5,s5,s7
  a2fa50:	03550a9b          	addshf	s5,a0,s5,sll,1
  a2fa54:	000adb03          	lhu	s6,0(s5)
  a2fa58:	00c97763          	bgeu	s2,a2,a2fa66 <LzmaDec_DecodeReal2+0x4c4>
  a2fa5c:	2304                	lbu	s1,0(a4)
  a2fa5e:	0922                	slli	s2,s2,0x8
  a2fa60:	0705                	addi	a4,a4,1
  a2fa62:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a2fa66:	00b95493          	srli	s1,s2,0xb
  a2fa6a:	036484b3          	mul	s1,s1,s6
  a2fa6e:	0686                	slli	a3,a3,0x1
  a2fa70:	1297f563          	bgeu	a5,s1,a2fb9a <LzmaDec_DecodeReal2+0x5f8>
  a2fa74:	41680933          	sub	s2,a6,s6
  a2fa78:	4b2b0b1b          	addshf	s6,s6,s2,srl,5
  a2fa7c:	fff34b93          	not	s7,t1
  a2fa80:	016a9023          	sh	s6,0(s5)
  a2fa84:	018bfbb3          	and	s7,s7,s8
  a2fa88:	0306                	slli	t1,t1,0x1
  a2fa8a:	01737c33          	and	s8,t1,s7
  a2fa8e:	00db8ab3          	add	s5,s7,a3
  a2fa92:	9ae2                	add	s5,s5,s8
  a2fa94:	03550a9b          	addshf	s5,a0,s5,sll,1
  a2fa98:	000adb03          	lhu	s6,0(s5)
  a2fa9c:	00c4f863          	bgeu	s1,a2,a2faac <LzmaDec_DecodeReal2+0x50a>
  a2faa0:	00074903          	lbu	s2,0(a4)
  a2faa4:	04a2                	slli	s1,s1,0x8
  a2faa6:	0705                	addi	a4,a4,1
  a2faa8:	10f9279b          	orshf	a5,s2,a5,sll,8
  a2faac:	00b4d913          	srli	s2,s1,0xb
  a2fab0:	03690933          	mul	s2,s2,s6
  a2fab4:	0686                	slli	a3,a3,0x1
  a2fab6:	0f27fb63          	bgeu	a5,s2,a2fbac <LzmaDec_DecodeReal2+0x60a>
  a2faba:	416804b3          	sub	s1,a6,s6
  a2fabe:	4a9b0b1b          	addshf	s6,s6,s1,srl,5
  a2fac2:	fff34c13          	not	s8,t1
  a2fac6:	016a9023          	sh	s6,0(s5)
  a2faca:	017c7c33          	and	s8,s8,s7
  a2face:	0306                	slli	t1,t1,0x1
  a2fad0:	01837ab3          	and	s5,t1,s8
  a2fad4:	00dc0b33          	add	s6,s8,a3
  a2fad8:	9b56                	add	s6,s6,s5
  a2fada:	03650b1b          	addshf	s6,a0,s6,sll,1
  a2fade:	000b5b83          	lhu	s7,0(s6) # 1000 <ccause+0x3e>
  a2fae2:	00c97763          	bgeu	s2,a2,a2faf0 <LzmaDec_DecodeReal2+0x54e>
  a2fae6:	2304                	lbu	s1,0(a4)
  a2fae8:	0922                	slli	s2,s2,0x8
  a2faea:	0705                	addi	a4,a4,1
  a2faec:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a2faf0:	00b95493          	srli	s1,s2,0xb
  a2faf4:	037484b3          	mul	s1,s1,s7
  a2faf8:	0686                	slli	a3,a3,0x1
  a2fafa:	0c97f363          	bgeu	a5,s1,a2fbc0 <LzmaDec_DecodeReal2+0x61e>
  a2fafe:	41780933          	sub	s2,a6,s7
  a2fb02:	4b2b8b9b          	addshf	s7,s7,s2,srl,5
  a2fb06:	fff34a93          	not	s5,t1
  a2fb0a:	017b1023          	sh	s7,0(s6)
  a2fb0e:	018afab3          	and	s5,s5,s8
  a2fb12:	026ac31b          	andshf	t1,s5,t1,sll,1
  a2fb16:	9ab6                	add	s5,s5,a3
  a2fb18:	9356                	add	t1,t1,s5
  a2fb1a:	0265051b          	addshf	a0,a0,t1,sll,1
  a2fb1e:	00055303          	lhu	t1,0(a0)
  a2fb22:	00c4f863          	bgeu	s1,a2,a2fb32 <LzmaDec_DecodeReal2+0x590>
  a2fb26:	00074903          	lbu	s2,0(a4)
  a2fb2a:	04a2                	slli	s1,s1,0x8
  a2fb2c:	0705                	addi	a4,a4,1
  a2fb2e:	10f9279b          	orshf	a5,s2,a5,sll,8
  a2fb32:	00b4d913          	srli	s2,s1,0xb
  a2fb36:	02690933          	mul	s2,s2,t1
  a2fb3a:	0686                	slli	a3,a3,0x1
  a2fb3c:	0927fb63          	bgeu	a5,s2,a2fbd2 <LzmaDec_DecodeReal2+0x630>
  a2fb40:	406804b3          	sub	s1,a6,t1
  a2fb44:	4a93031b          	addshf	t1,t1,s1,srl,5
  a2fb48:	00651023          	sh	t1,0(a0)
  a2fb4c:	bb21                	j	a2f864 <LzmaDec_DecodeReal2+0x2c2>
  a2fb4e:	4b29191b          	subshf	s2,s2,s2,srl,5
  a2fb52:	8f85                	sub	a5,a5,s1
  a2fb54:	012a9023          	sh	s2,0(s5)
  a2fb58:	409684b3          	sub	s1,a3,s1
  a2fb5c:	468d                	li	a3,3
  a2fb5e:	bd19                	j	a2f974 <LzmaDec_DecodeReal2+0x3d2>
  a2fb60:	4b6b1b1b          	subshf	s6,s6,s6,srl,5
  a2fb64:	412787b3          	sub	a5,a5,s2
  a2fb68:	016a9023          	sh	s6,0(s5)
  a2fb6c:	0685                	addi	a3,a3,1
  a2fb6e:	41248933          	sub	s2,s1,s2
  a2fb72:	b5a1                	j	a2f9ba <LzmaDec_DecodeReal2+0x418>
  a2fb74:	4b6b1b1b          	subshf	s6,s6,s6,srl,5
  a2fb78:	8f85                	sub	a5,a5,s1
  a2fb7a:	016a9023          	sh	s6,0(s5)
  a2fb7e:	0685                	addi	a3,a3,1
  a2fb80:	409904b3          	sub	s1,s2,s1
  a2fb84:	bdad                	j	a2f9fe <LzmaDec_DecodeReal2+0x45c>
  a2fb86:	4b6b1b1b          	subshf	s6,s6,s6,srl,5
  a2fb8a:	412787b3          	sub	a5,a5,s2
  a2fb8e:	016a9023          	sh	s6,0(s5)
  a2fb92:	0685                	addi	a3,a3,1
  a2fb94:	41248933          	sub	s2,s1,s2
  a2fb98:	b575                	j	a2fa44 <LzmaDec_DecodeReal2+0x4a2>
  a2fb9a:	4b6b1b1b          	subshf	s6,s6,s6,srl,5
  a2fb9e:	8f85                	sub	a5,a5,s1
  a2fba0:	016a9023          	sh	s6,0(s5)
  a2fba4:	0685                	addi	a3,a3,1
  a2fba6:	409904b3          	sub	s1,s2,s1
  a2fbaa:	bdf9                	j	a2fa88 <LzmaDec_DecodeReal2+0x4e6>
  a2fbac:	4b6b1b1b          	subshf	s6,s6,s6,srl,5
  a2fbb0:	412787b3          	sub	a5,a5,s2
  a2fbb4:	016a9023          	sh	s6,0(s5)
  a2fbb8:	0685                	addi	a3,a3,1
  a2fbba:	41248933          	sub	s2,s1,s2
  a2fbbe:	bf01                	j	a2face <LzmaDec_DecodeReal2+0x52c>
  a2fbc0:	4b7b9b9b          	subshf	s7,s7,s7,srl,5
  a2fbc4:	8f85                	sub	a5,a5,s1
  a2fbc6:	017b1023          	sh	s7,0(s6)
  a2fbca:	0685                	addi	a3,a3,1
  a2fbcc:	409904b3          	sub	s1,s2,s1
  a2fbd0:	b789                	j	a2fb12 <LzmaDec_DecodeReal2+0x570>
  a2fbd2:	4a63131b          	subshf	t1,t1,t1,srl,5
  a2fbd6:	412787b3          	sub	a5,a5,s2
  a2fbda:	00651023          	sh	t1,0(a0)
  a2fbde:	b335                	j	a2f90a <LzmaDec_DecodeReal2+0x368>
  a2fbe0:	4a63131b          	subshf	t1,t1,t1,srl,5
  a2fbe4:	006a9023          	sh	t1,0(s5)
  a2fbe8:	010f0313          	addi	t1,t5,16
  a2fbec:	0306                	slli	t1,t1,0x1
  a2fbee:	006f8b33          	add	s6,t6,t1
  a2fbf2:	40d90933          	sub	s2,s2,a3
  a2fbf6:	8f95                	sub	a5,a5,a3
  a2fbf8:	000b5a83          	lhu	s5,0(s6)
  a2fbfc:	00c97763          	bgeu	s2,a2,a2fc0a <LzmaDec_DecodeReal2+0x668>
  a2fc00:	2314                	lbu	a3,0(a4)
  a2fc02:	0922                	slli	s2,s2,0x8
  a2fc04:	0705                	addi	a4,a4,1
  a2fc06:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2fc0a:	00b95693          	srli	a3,s2,0xb
  a2fc0e:	035686b3          	mul	a3,a3,s5
  a2fc12:	12d7f363          	bgeu	a5,a3,a2fd38 <LzmaDec_DecodeReal2+0x796>
  a2fc16:	415808b3          	sub	a7,a6,s5
  a2fc1a:	5322                	lw	t1,40(sp)
  a2fc1c:	4b1a8a9b          	addshf	s5,s5,a7,srl,5
  a2fc20:	015b1023          	sh	s5,0(s6)
  a2fc24:	0f31                	addi	t5,t5,12
  a2fc26:	00035483          	lhu	s1,0(t1)
  a2fc2a:	00c6f863          	bgeu	a3,a2,a2fc3a <LzmaDec_DecodeReal2+0x698>
  a2fc2e:	00074883          	lbu	a7,0(a4)
  a2fc32:	06a2                	slli	a3,a3,0x8
  a2fc34:	0705                	addi	a4,a4,1
  a2fc36:	10f8a79b          	orshf	a5,a7,a5,sll,8
  a2fc3a:	00b6d893          	srli	a7,a3,0xb
  a2fc3e:	029888b3          	mul	a7,a7,s1
  a2fc42:	2917f463          	bgeu	a5,a7,a2feca <LzmaDec_DecodeReal2+0x928>
  a2fc46:	409806b3          	sub	a3,a6,s1
  a2fc4a:	4ad4849b          	addshf	s1,s1,a3,srl,5
  a2fc4e:	00931023          	sh	s1,0(t1)
  a2fc52:	02a3051b          	addshf	a0,t1,a0,sll,1
  a2fc56:	2136                	lhu	a3,2(a0)
  a2fc58:	00c8f863          	bgeu	a7,a2,a2fc68 <LzmaDec_DecodeReal2+0x6c6>
  a2fc5c:	00074303          	lbu	t1,0(a4)
  a2fc60:	08a2                	slli	a7,a7,0x8
  a2fc62:	0705                	addi	a4,a4,1
  a2fc64:	10f3279b          	orshf	a5,t1,a5,sll,8
  a2fc68:	00b8d493          	srli	s1,a7,0xb
  a2fc6c:	02d484b3          	mul	s1,s1,a3
  a2fc70:	2297f263          	bgeu	a5,s1,a2fe94 <LzmaDec_DecodeReal2+0x8f2>
  a2fc74:	40d808b3          	sub	a7,a6,a3
  a2fc78:	4b16869b          	addshf	a3,a3,a7,srl,5
  a2fc7c:	a136                	sh	a3,2(a0)
  a2fc7e:	4689                	li	a3,2
  a2fc80:	0686                	slli	a3,a3,0x1
  a2fc82:	00d50933          	add	s2,a0,a3
  a2fc86:	00095303          	lhu	t1,0(s2)
  a2fc8a:	00c4f863          	bgeu	s1,a2,a2fc9a <LzmaDec_DecodeReal2+0x6f8>
  a2fc8e:	00074883          	lbu	a7,0(a4)
  a2fc92:	04a2                	slli	s1,s1,0x8
  a2fc94:	0705                	addi	a4,a4,1
  a2fc96:	10f8a79b          	orshf	a5,a7,a5,sll,8
  a2fc9a:	00b4d893          	srli	a7,s1,0xb
  a2fc9e:	026888b3          	mul	a7,a7,t1
  a2fca2:	2117f163          	bgeu	a5,a7,a2fea4 <LzmaDec_DecodeReal2+0x902>
  a2fca6:	406804b3          	sub	s1,a6,t1
  a2fcaa:	4a93031b          	addshf	t1,t1,s1,srl,5
  a2fcae:	00691023          	sh	t1,0(s2)
  a2fcb2:	0686                	slli	a3,a3,0x1
  a2fcb4:	9536                	add	a0,a0,a3
  a2fcb6:	2106                	lhu	s1,0(a0)
  a2fcb8:	00c8f863          	bgeu	a7,a2,a2fcc8 <LzmaDec_DecodeReal2+0x726>
  a2fcbc:	00074303          	lbu	t1,0(a4)
  a2fcc0:	08a2                	slli	a7,a7,0x8
  a2fcc2:	0705                	addi	a4,a4,1
  a2fcc4:	10f3279b          	orshf	a5,t1,a5,sll,8
  a2fcc8:	00b8d913          	srli	s2,a7,0xb
  a2fccc:	02990933          	mul	s2,s2,s1
  a2fcd0:	1f27f463          	bgeu	a5,s2,a2feb8 <LzmaDec_DecodeReal2+0x916>
  a2fcd4:	409808b3          	sub	a7,a6,s1
  a2fcd8:	4b14849b          	addshf	s1,s1,a7,srl,5
  a2fcdc:	a106                	sh	s1,0(a0)
  a2fcde:	452d                	li	a0,11
  a2fce0:	ff868893          	addi	a7,a3,-8
  a2fce4:	35e56a63          	bltu	a0,t5,a30038 <LzmaDec_DecodeReal2+0xa96>
  a2fce8:	41d586b3          	sub	a3,a1,t4
  a2fcec:	0889                	addi	a7,a7,2
  a2fcee:	68068863          	beqz	a3,a3037e <LzmaDec_DecodeReal2+0xddc>
  a2fcf2:	8546                	mv	a0,a7
  a2fcf4:	0116f363          	bgeu	a3,a7,a2fcfa <LzmaDec_DecodeReal2+0x758>
  a2fcf8:	8536                	mv	a0,a3
  a2fcfa:	405e8333          	sub	t1,t4,t0
  a2fcfe:	4681                	li	a3,0
  a2fd00:	005ef363          	bgeu	t4,t0,a2fd06 <LzmaDec_DecodeReal2+0x764>
  a2fd04:	86e6                	mv	a3,s9
  a2fd06:	969a                	add	a3,a3,t1
  a2fd08:	40dc8333          	sub	t1,s9,a3
  a2fd0c:	93aa                	add	t2,t2,a0
  a2fd0e:	40a888b3          	sub	a7,a7,a0
  a2fd12:	00ae84b3          	add	s1,t4,a0
  a2fd16:	72a36263          	bltu	t1,a0,a3043a <LzmaDec_DecodeReal2+0xe98>
  a2fd1a:	01da0333          	add	t1,s4,t4
  a2fd1e:	41d686b3          	sub	a3,a3,t4
  a2fd22:	951a                	add	a0,a0,t1
  a2fd24:	8ea6                	mv	t4,s1
  a2fd26:	00d304b3          	add	s1,t1,a3
  a2fd2a:	2084                	lbu	s1,0(s1)
  a2fd2c:	0305                	addi	t1,t1,1
  a2fd2e:	fe930fa3          	sb	s1,-1(t1)
  a2fd32:	fe651ae3          	bne	a0,t1,a2fd26 <LzmaDec_DecodeReal2+0x784>
  a2fd36:	be1d                	j	a2f86c <LzmaDec_DecodeReal2+0x2ca>
  a2fd38:	4b5a9a9b          	subshf	s5,s5,s5,srl,5
  a2fd3c:	015b1023          	sh	s5,0(s6)
  a2fd40:	01830b13          	addi	s6,t1,24
  a2fd44:	9b7e                	add	s6,s6,t6
  a2fd46:	40d90933          	sub	s2,s2,a3
  a2fd4a:	8f95                	sub	a5,a5,a3
  a2fd4c:	000b5a83          	lhu	s5,0(s6)
  a2fd50:	00c97763          	bgeu	s2,a2,a2fd5e <LzmaDec_DecodeReal2+0x7bc>
  a2fd54:	2314                	lbu	a3,0(a4)
  a2fd56:	0922                	slli	s2,s2,0x8
  a2fd58:	0705                	addi	a4,a4,1
  a2fd5a:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2fd5e:	00b95693          	srli	a3,s2,0xb
  a2fd62:	035686b3          	mul	a3,a3,s5
  a2fd66:	08d7f763          	bgeu	a5,a3,a2fdf4 <LzmaDec_DecodeReal2+0x852>
  a2fd6a:	41580333          	sub	t1,a6,s5
  a2fd6e:	4a6a8a9b          	addshf	s5,s5,t1,srl,5
  a2fd72:	4342                	lw	t1,16(sp)
  a2fd74:	015b1023          	sh	s5,0(s6)
  a2fd78:	949a                	add	s1,s1,t1
  a2fd7a:	94fe                	add	s1,s1,t6
  a2fd7c:	0004d303          	lhu	t1,0(s1)
  a2fd80:	00c6f863          	bgeu	a3,a2,a2fd90 <LzmaDec_DecodeReal2+0x7ee>
  a2fd84:	00074903          	lbu	s2,0(a4)
  a2fd88:	06a2                	slli	a3,a3,0x8
  a2fd8a:	0705                	addi	a4,a4,1
  a2fd8c:	10f9279b          	orshf	a5,s2,a5,sll,8
  a2fd90:	00b6d913          	srli	s2,a3,0xb
  a2fd94:	02690933          	mul	s2,s2,t1
  a2fd98:	0327fb63          	bgeu	a5,s2,a2fdce <LzmaDec_DecodeReal2+0x82c>
  a2fd9c:	406806b3          	sub	a3,a6,t1
  a2fda0:	4ad3031b          	addshf	t1,t1,a3,srl,5
  a2fda4:	00649023          	sh	t1,0(s1)
  a2fda8:	405e86b3          	sub	a3,t4,t0
  a2fdac:	4501                	li	a0,0
  a2fdae:	005ef363          	bgeu	t4,t0,a2fdb4 <LzmaDec_DecodeReal2+0x812>
  a2fdb2:	8566                	mv	a0,s9
  a2fdb4:	96d2                	add	a3,a3,s4
  a2fdb6:	96aa                	add	a3,a3,a0
  a2fdb8:	2288                	lbu	a0,0(a3)
  a2fdba:	01da06b3          	add	a3,s4,t4
  a2fdbe:	0385                	addi	t2,t2,1
  a2fdc0:	a288                	sb	a0,0(a3)
  a2fdc2:	0e85                	addi	t4,t4,1
  a2fdc4:	469d                	li	a3,7
  a2fdc6:	68df7b63          	bgeu	t5,a3,a3045c <LzmaDec_DecodeReal2+0xeba>
  a2fdca:	4f25                	li	t5,9
  a2fdcc:	b445                	j	a2f86c <LzmaDec_DecodeReal2+0x2ca>
  a2fdce:	88a2                	mv	a7,s0
  a2fdd0:	4a63131b          	subshf	t1,t1,t1,srl,5
  a2fdd4:	8416                	mv	s0,t0
  a2fdd6:	412686b3          	sub	a3,a3,s2
  a2fdda:	412787b3          	sub	a5,a5,s2
  a2fdde:	00649023          	sh	t1,0(s1)
  a2fde2:	82c6                	mv	t0,a7
  a2fde4:	072f7b3b          	bgeui	t5,7,a2fe90 <LzmaDec_DecodeReal2+0x8ee>
  a2fde8:	4f21                	li	t5,8
  a2fdea:	8896                	mv	a7,t0
  a2fdec:	5312                	lw	t1,36(sp)
  a2fdee:	82a2                	mv	t0,s0
  a2fdf0:	8446                	mv	s0,a7
  a2fdf2:	bd15                	j	a2fc26 <LzmaDec_DecodeReal2+0x684>
  a2fdf4:	4b5a9a9b          	subshf	s5,s5,s5,srl,5
  a2fdf8:	03030493          	addi	s1,t1,48
  a2fdfc:	015b1023          	sh	s5,0(s6)
  a2fe00:	94fe                	add	s1,s1,t6
  a2fe02:	40d90933          	sub	s2,s2,a3
  a2fe06:	8f95                	sub	a5,a5,a3
  a2fe08:	0004d883          	lhu	a7,0(s1)
  a2fe0c:	00c97763          	bgeu	s2,a2,a2fe1a <LzmaDec_DecodeReal2+0x878>
  a2fe10:	2314                	lbu	a3,0(a4)
  a2fe12:	0922                	slli	s2,s2,0x8
  a2fe14:	0705                	addi	a4,a4,1
  a2fe16:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2fe1a:	00b95693          	srli	a3,s2,0xb
  a2fe1e:	031686b3          	mul	a3,a3,a7
  a2fe22:	00d7f963          	bgeu	a5,a3,a2fe34 <LzmaDec_DecodeReal2+0x892>
  a2fe26:	41180333          	sub	t1,a6,a7
  a2fe2a:	4a68889b          	addshf	a7,a7,t1,srl,5
  a2fe2e:	01149023          	sh	a7,0(s1)
  a2fe32:	bf4d                	j	a2fde4 <LzmaDec_DecodeReal2+0x842>
  a2fe34:	4b18989b          	subshf	a7,a7,a7,srl,5
  a2fe38:	04830313          	addi	t1,t1,72
  a2fe3c:	01149023          	sh	a7,0(s1)
  a2fe40:	937e                	add	t1,t1,t6
  a2fe42:	40d90933          	sub	s2,s2,a3
  a2fe46:	8f95                	sub	a5,a5,a3
  a2fe48:	00035883          	lhu	a7,0(t1)
  a2fe4c:	00c97763          	bgeu	s2,a2,a2fe5a <LzmaDec_DecodeReal2+0x8b8>
  a2fe50:	2314                	lbu	a3,0(a4)
  a2fe52:	0922                	slli	s2,s2,0x8
  a2fe54:	0705                	addi	a4,a4,1
  a2fe56:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2fe5a:	00b95693          	srli	a3,s2,0xb
  a2fe5e:	031686b3          	mul	a3,a3,a7
  a2fe62:	00d7fc63          	bgeu	a5,a3,a2fe7a <LzmaDec_DecodeReal2+0x8d8>
  a2fe66:	411804b3          	sub	s1,a6,a7
  a2fe6a:	4a98889b          	addshf	a7,a7,s1,srl,5
  a2fe6e:	01131023          	sh	a7,0(t1)
  a2fe72:	88a2                	mv	a7,s0
  a2fe74:	844e                	mv	s0,s3
  a2fe76:	89c6                	mv	s3,a7
  a2fe78:	b7b5                	j	a2fde4 <LzmaDec_DecodeReal2+0x842>
  a2fe7a:	4b18989b          	subshf	a7,a7,a7,srl,5
  a2fe7e:	8f95                	sub	a5,a5,a3
  a2fe80:	01131023          	sh	a7,0(t1)
  a2fe84:	40d906b3          	sub	a3,s2,a3
  a2fe88:	88a2                	mv	a7,s0
  a2fe8a:	846a                	mv	s0,s10
  a2fe8c:	8d4e                	mv	s10,s3
  a2fe8e:	b7e5                	j	a2fe76 <LzmaDec_DecodeReal2+0x8d4>
  a2fe90:	4f2d                	li	t5,11
  a2fe92:	bfa1                	j	a2fdea <LzmaDec_DecodeReal2+0x848>
  a2fe94:	4ad6969b          	subshf	a3,a3,a3,srl,5
  a2fe98:	8f85                	sub	a5,a5,s1
  a2fe9a:	a136                	sh	a3,2(a0)
  a2fe9c:	409884b3          	sub	s1,a7,s1
  a2fea0:	468d                	li	a3,3
  a2fea2:	bbf9                	j	a2fc80 <LzmaDec_DecodeReal2+0x6de>
  a2fea4:	4a63131b          	subshf	t1,t1,t1,srl,5
  a2fea8:	411787b3          	sub	a5,a5,a7
  a2feac:	00691023          	sh	t1,0(s2)
  a2feb0:	0685                	addi	a3,a3,1
  a2feb2:	411488b3          	sub	a7,s1,a7
  a2feb6:	bbf5                	j	a2fcb2 <LzmaDec_DecodeReal2+0x710>
  a2feb8:	4a94949b          	subshf	s1,s1,s1,srl,5
  a2febc:	412787b3          	sub	a5,a5,s2
  a2fec0:	a106                	sh	s1,0(a0)
  a2fec2:	0685                	addi	a3,a3,1
  a2fec4:	41288933          	sub	s2,a7,s2
  a2fec8:	bd19                	j	a2fcde <LzmaDec_DecodeReal2+0x73c>
  a2feca:	4a94949b          	subshf	s1,s1,s1,srl,5
  a2fece:	00931023          	sh	s1,0(t1)
  a2fed2:	41168933          	sub	s2,a3,a7
  a2fed6:	411787b3          	sub	a5,a5,a7
  a2feda:	01035483          	lhu	s1,16(t1)
  a2fede:	00c97763          	bgeu	s2,a2,a2feec <LzmaDec_DecodeReal2+0x94a>
  a2fee2:	2314                	lbu	a3,0(a4)
  a2fee4:	0922                	slli	s2,s2,0x8
  a2fee6:	0705                	addi	a4,a4,1
  a2fee8:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2feec:	00b95893          	srli	a7,s2,0xb
  a2fef0:	029888b3          	mul	a7,a7,s1
  a2fef4:	0f17f263          	bgeu	a5,a7,a2ffd8 <LzmaDec_DecodeReal2+0xa36>
  a2fef8:	409806b3          	sub	a3,a6,s1
  a2fefc:	4ad4849b          	addshf	s1,s1,a3,srl,5
  a2ff00:	0521                	addi	a0,a0,8
  a2ff02:	00931823          	sh	s1,16(t1)
  a2ff06:	02a3031b          	addshf	t1,t1,a0,sll,1
  a2ff0a:	00235683          	lhu	a3,2(t1)
  a2ff0e:	00c8f763          	bgeu	a7,a2,a2ff1c <LzmaDec_DecodeReal2+0x97a>
  a2ff12:	2308                	lbu	a0,0(a4)
  a2ff14:	08a2                	slli	a7,a7,0x8
  a2ff16:	0705                	addi	a4,a4,1
  a2ff18:	10f5279b          	orshf	a5,a0,a5,sll,8
  a2ff1c:	00b8d493          	srli	s1,a7,0xb
  a2ff20:	02d484b3          	mul	s1,s1,a3
  a2ff24:	0697fe63          	bgeu	a5,s1,a2ffa0 <LzmaDec_DecodeReal2+0x9fe>
  a2ff28:	40d80533          	sub	a0,a6,a3
  a2ff2c:	4aa6869b          	addshf	a3,a3,a0,srl,5
  a2ff30:	00d31123          	sh	a3,2(t1)
  a2ff34:	4889                	li	a7,2
  a2ff36:	0886                	slli	a7,a7,0x1
  a2ff38:	01130933          	add	s2,t1,a7
  a2ff3c:	00095503          	lhu	a0,0(s2)
  a2ff40:	00c4f763          	bgeu	s1,a2,a2ff4e <LzmaDec_DecodeReal2+0x9ac>
  a2ff44:	2314                	lbu	a3,0(a4)
  a2ff46:	04a2                	slli	s1,s1,0x8
  a2ff48:	0705                	addi	a4,a4,1
  a2ff4a:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a2ff4e:	00b4d693          	srli	a3,s1,0xb
  a2ff52:	02a686b3          	mul	a3,a3,a0
  a2ff56:	04d7fe63          	bgeu	a5,a3,a2ffb2 <LzmaDec_DecodeReal2+0xa10>
  a2ff5a:	40a804b3          	sub	s1,a6,a0
  a2ff5e:	4a95051b          	addshf	a0,a0,s1,srl,5
  a2ff62:	00a91023          	sh	a0,0(s2)
  a2ff66:	0886                	slli	a7,a7,0x1
  a2ff68:	9346                	add	t1,t1,a7
  a2ff6a:	00035483          	lhu	s1,0(t1)
  a2ff6e:	00c6f763          	bgeu	a3,a2,a2ff7c <LzmaDec_DecodeReal2+0x9da>
  a2ff72:	2308                	lbu	a0,0(a4)
  a2ff74:	06a2                	slli	a3,a3,0x8
  a2ff76:	0705                	addi	a4,a4,1
  a2ff78:	10f5279b          	orshf	a5,a0,a5,sll,8
  a2ff7c:	00b6d913          	srli	s2,a3,0xb
  a2ff80:	02990933          	mul	s2,s2,s1
  a2ff84:	0527f063          	bgeu	a5,s2,a2ffc4 <LzmaDec_DecodeReal2+0xa22>
  a2ff88:	409806b3          	sub	a3,a6,s1
  a2ff8c:	4ad4849b          	addshf	s1,s1,a3,srl,5
  a2ff90:	00931023          	sh	s1,0(t1)
  a2ff94:	46ad                	li	a3,11
  a2ff96:	d5e6f9e3          	bgeu	a3,t5,a2fce8 <LzmaDec_DecodeReal2+0x746>
  a2ff9a:	20000313          	li	t1,512
  a2ff9e:	a06d                	j	a30048 <LzmaDec_DecodeReal2+0xaa6>
  a2ffa0:	4ad6969b          	subshf	a3,a3,a3,srl,5
  a2ffa4:	8f85                	sub	a5,a5,s1
  a2ffa6:	00d31123          	sh	a3,2(t1)
  a2ffaa:	409884b3          	sub	s1,a7,s1
  a2ffae:	488d                	li	a7,3
  a2ffb0:	b759                	j	a2ff36 <LzmaDec_DecodeReal2+0x994>
  a2ffb2:	4aa5151b          	subshf	a0,a0,a0,srl,5
  a2ffb6:	8f95                	sub	a5,a5,a3
  a2ffb8:	00a91023          	sh	a0,0(s2)
  a2ffbc:	0885                	addi	a7,a7,1
  a2ffbe:	40d486b3          	sub	a3,s1,a3
  a2ffc2:	b755                	j	a2ff66 <LzmaDec_DecodeReal2+0x9c4>
  a2ffc4:	4a94949b          	subshf	s1,s1,s1,srl,5
  a2ffc8:	412787b3          	sub	a5,a5,s2
  a2ffcc:	00931023          	sh	s1,0(t1)
  a2ffd0:	0885                	addi	a7,a7,1
  a2ffd2:	41268933          	sub	s2,a3,s2
  a2ffd6:	bf7d                	j	a2ff94 <LzmaDec_DecodeReal2+0x9f2>
  a2ffd8:	4a94949b          	subshf	s1,s1,s1,srl,5
  a2ffdc:	41190933          	sub	s2,s2,a7
  a2ffe0:	411787b3          	sub	a5,a5,a7
  a2ffe4:	00931823          	sh	s1,16(t1)
  a2ffe8:	4885                	li	a7,1
  a2ffea:	0ff00a93          	li	s5,255
  a2ffee:	0886                	slli	a7,a7,0x1
  a2fff0:	20088493          	addi	s1,a7,512
  a2fff4:	949a                	add	s1,s1,t1
  a2fff6:	2096                	lhu	a3,0(s1)
  a2fff8:	00c97763          	bgeu	s2,a2,a30006 <LzmaDec_DecodeReal2+0xa64>
  a2fffc:	2308                	lbu	a0,0(a4)
  a2fffe:	0922                	slli	s2,s2,0x8
  a30000:	0705                	addi	a4,a4,1
  a30002:	10f5279b          	orshf	a5,a0,a5,sll,8
  a30006:	00b95513          	srli	a0,s2,0xb
  a3000a:	02d50533          	mul	a0,a0,a3
  a3000e:	00a7fd63          	bgeu	a5,a0,a30028 <LzmaDec_DecodeReal2+0xa86>
  a30012:	40d80933          	sub	s2,a6,a3
  a30016:	4b26869b          	addshf	a3,a3,s2,srl,5
  a3001a:	a096                	sh	a3,0(s1)
  a3001c:	892a                	mv	s2,a0
  a3001e:	fd1af8e3          	bgeu	s5,a7,a2ffee <LzmaDec_DecodeReal2+0xa4c>
  a30022:	f1088893          	addi	a7,a7,-240
  a30026:	b7bd                	j	a2ff94 <LzmaDec_DecodeReal2+0x9f2>
  a30028:	4ad6969b          	subshf	a3,a3,a3,srl,5
  a3002c:	40a90933          	sub	s2,s2,a0
  a30030:	8f89                	sub	a5,a5,a0
  a30032:	a096                	sh	a3,0(s1)
  a30034:	0885                	addi	a7,a7,1
  a30036:	b7e5                	j	a3001e <LzmaDec_DecodeReal2+0xa7c>
  a30038:	450d                	li	a0,3
  a3003a:	20000313          	li	t1,512
  a3003e:	01156563          	bltu	a0,a7,a30048 <LzmaDec_DecodeReal2+0xaa6>
  a30042:	16e5                	addi	a3,a3,-7
  a30044:	00769313          	slli	t1,a3,0x7
  a30048:	937e                	add	t1,t1,t6
  a3004a:	00235683          	lhu	a3,2(t1)
  a3004e:	00c97763          	bgeu	s2,a2,a3005c <LzmaDec_DecodeReal2+0xaba>
  a30052:	2308                	lbu	a0,0(a4)
  a30054:	0922                	slli	s2,s2,0x8
  a30056:	0705                	addi	a4,a4,1
  a30058:	10f5279b          	orshf	a5,a0,a5,sll,8
  a3005c:	00b95493          	srli	s1,s2,0xb
  a30060:	02d484b3          	mul	s1,s1,a3
  a30064:	1897fc63          	bgeu	a5,s1,a301fc <LzmaDec_DecodeReal2+0xc5a>
  a30068:	40d80533          	sub	a0,a6,a3
  a3006c:	4aa6869b          	addshf	a3,a3,a0,srl,5
  a30070:	00d31123          	sh	a3,2(t1)
  a30074:	4689                	li	a3,2
  a30076:	0686                	slli	a3,a3,0x1
  a30078:	00d30ab3          	add	s5,t1,a3
  a3007c:	000ad903          	lhu	s2,0(s5)
  a30080:	00c4f763          	bgeu	s1,a2,a3008e <LzmaDec_DecodeReal2+0xaec>
  a30084:	2308                	lbu	a0,0(a4)
  a30086:	04a2                	slli	s1,s1,0x8
  a30088:	0705                	addi	a4,a4,1
  a3008a:	10f5279b          	orshf	a5,a0,a5,sll,8
  a3008e:	00b4d513          	srli	a0,s1,0xb
  a30092:	03250533          	mul	a0,a0,s2
  a30096:	16a7fc63          	bgeu	a5,a0,a3020e <LzmaDec_DecodeReal2+0xc6c>
  a3009a:	412804b3          	sub	s1,a6,s2
  a3009e:	4a99091b          	addshf	s2,s2,s1,srl,5
  a300a2:	012a9023          	sh	s2,0(s5)
  a300a6:	0686                	slli	a3,a3,0x1
  a300a8:	00d30ab3          	add	s5,t1,a3
  a300ac:	000ad903          	lhu	s2,0(s5)
  a300b0:	00c57763          	bgeu	a0,a2,a300be <LzmaDec_DecodeReal2+0xb1c>
  a300b4:	2304                	lbu	s1,0(a4)
  a300b6:	0522                	slli	a0,a0,0x8
  a300b8:	0705                	addi	a4,a4,1
  a300ba:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a300be:	00b55493          	srli	s1,a0,0xb
  a300c2:	032484b3          	mul	s1,s1,s2
  a300c6:	1497fd63          	bgeu	a5,s1,a30220 <LzmaDec_DecodeReal2+0xc7e>
  a300ca:	41280533          	sub	a0,a6,s2
  a300ce:	4aa9091b          	addshf	s2,s2,a0,srl,5
  a300d2:	012a9023          	sh	s2,0(s5)
  a300d6:	0686                	slli	a3,a3,0x1
  a300d8:	00d30ab3          	add	s5,t1,a3
  a300dc:	000ad903          	lhu	s2,0(s5)
  a300e0:	00c4f763          	bgeu	s1,a2,a300ee <LzmaDec_DecodeReal2+0xb4c>
  a300e4:	2308                	lbu	a0,0(a4)
  a300e6:	04a2                	slli	s1,s1,0x8
  a300e8:	0705                	addi	a4,a4,1
  a300ea:	10f5279b          	orshf	a5,a0,a5,sll,8
  a300ee:	00b4d513          	srli	a0,s1,0xb
  a300f2:	03250533          	mul	a0,a0,s2
  a300f6:	12a7fe63          	bgeu	a5,a0,a30232 <LzmaDec_DecodeReal2+0xc90>
  a300fa:	412804b3          	sub	s1,a6,s2
  a300fe:	4a99091b          	addshf	s2,s2,s1,srl,5
  a30102:	012a9023          	sh	s2,0(s5)
  a30106:	0686                	slli	a3,a3,0x1
  a30108:	00d30ab3          	add	s5,t1,a3
  a3010c:	000ad903          	lhu	s2,0(s5)
  a30110:	00c57763          	bgeu	a0,a2,a3011e <LzmaDec_DecodeReal2+0xb7c>
  a30114:	2304                	lbu	s1,0(a4)
  a30116:	0522                	slli	a0,a0,0x8
  a30118:	0705                	addi	a4,a4,1
  a3011a:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a3011e:	00b55493          	srli	s1,a0,0xb
  a30122:	032484b3          	mul	s1,s1,s2
  a30126:	1097ff63          	bgeu	a5,s1,a30244 <LzmaDec_DecodeReal2+0xca2>
  a3012a:	41280533          	sub	a0,a6,s2
  a3012e:	4aa9091b          	addshf	s2,s2,a0,srl,5
  a30132:	012a9023          	sh	s2,0(s5)
  a30136:	0686                	slli	a3,a3,0x1
  a30138:	9336                	add	t1,t1,a3
  a3013a:	00035503          	lhu	a0,0(t1)
  a3013e:	00c4f863          	bgeu	s1,a2,a3014e <LzmaDec_DecodeReal2+0xbac>
  a30142:	00074903          	lbu	s2,0(a4)
  a30146:	04a2                	slli	s1,s1,0x8
  a30148:	0705                	addi	a4,a4,1
  a3014a:	10f9279b          	orshf	a5,s2,a5,sll,8
  a3014e:	00b4d913          	srli	s2,s1,0xb
  a30152:	02a90933          	mul	s2,s2,a0
  a30156:	1127f063          	bgeu	a5,s2,a30256 <LzmaDec_DecodeReal2+0xcb4>
  a3015a:	40a804b3          	sub	s1,a6,a0
  a3015e:	4a95051b          	addshf	a0,a0,s1,srl,5
  a30162:	00a31023          	sh	a0,0(t1)
  a30166:	fc068693          	addi	a3,a3,-64
  a3016a:	450d                	li	a0,3
  a3016c:	06d57563          	bgeu	a0,a3,a301d6 <LzmaDec_DecodeReal2+0xc34>
  a30170:	0016f513          	andi	a0,a3,1
  a30174:	44b5                	li	s1,13
  a30176:	0016d313          	srli	t1,a3,0x1
  a3017a:	00256513          	ori	a0,a0,2
  a3017e:	10d4e063          	bltu	s1,a3,a3027e <LzmaDec_DecodeReal2+0xcdc>
  a30182:	137d                	addi	t1,t1,-1
  a30184:	00651533          	sll	a0,a0,t1
  a30188:	0505                	addi	a0,a0,1
  a3018a:	4b85                	li	s7,1
  a3018c:	76fd                	lui	a3,0xfffff
  a3018e:	30068693          	addi	a3,a3,768 # fffff300 <_gp_+0xff5cab44>
  a30192:	02a68b1b          	addshf	s6,a3,a0,sll,1
  a30196:	9b7e                	add	s6,s6,t6
  a30198:	000b5483          	lhu	s1,0(s6)
  a3019c:	00c97763          	bgeu	s2,a2,a301aa <LzmaDec_DecodeReal2+0xc08>
  a301a0:	2314                	lbu	a3,0(a4)
  a301a2:	0922                	slli	s2,s2,0x8
  a301a4:	0705                	addi	a4,a4,1
  a301a6:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a301aa:	00b95a93          	srli	s5,s2,0xb
  a301ae:	029a8ab3          	mul	s5,s5,s1
  a301b2:	001b9693          	slli	a3,s7,0x1
  a301b6:	0b57fa63          	bgeu	a5,s5,a3026a <LzmaDec_DecodeReal2+0xcc8>
  a301ba:	40980933          	sub	s2,a6,s1
  a301be:	4b24849b          	addshf	s1,s1,s2,srl,5
  a301c2:	009b1023          	sh	s1,0(s6)
  a301c6:	955e                	add	a0,a0,s7
  a301c8:	8956                	mv	s2,s5
  a301ca:	137d                	addi	t1,t1,-1
  a301cc:	8bb6                	mv	s7,a3
  a301ce:	fa031fe3          	bnez	t1,a3018c <LzmaDec_DecodeReal2+0xbea>
  a301d2:	40d506b3          	sub	a3,a0,a3
  a301d6:	44c9                	li	s1,18
  a301d8:	00168513          	addi	a0,a3,1
  a301dc:	25e4e863          	bltu	s1,t5,a3042c <LzmaDec_DecodeReal2+0xe8a>
  a301e0:	4f1d                	li	t5,7
  a301e2:	831e                	mv	t1,t2
  a301e4:	000d8363          	beqz	s11,a301ea <LzmaDec_DecodeReal2+0xc48>
  a301e8:	836e                	mv	t1,s11
  a301ea:	2466e363          	bltu	a3,t1,a30430 <LzmaDec_DecodeReal2+0xe8e>
  a301ee:	8d4e                	mv	s10,s3
  a301f0:	20288893          	addi	a7,a7,514
  a301f4:	89a2                	mv	s3,s0
  a301f6:	8416                	mv	s0,t0
  a301f8:	82aa                	mv	t0,a0
  a301fa:	a251                	j	a3037e <LzmaDec_DecodeReal2+0xddc>
  a301fc:	4ad6969b          	subshf	a3,a3,a3,srl,5
  a30200:	8f85                	sub	a5,a5,s1
  a30202:	00d31123          	sh	a3,2(t1)
  a30206:	409904b3          	sub	s1,s2,s1
  a3020a:	468d                	li	a3,3
  a3020c:	b5ad                	j	a30076 <LzmaDec_DecodeReal2+0xad4>
  a3020e:	4b29191b          	subshf	s2,s2,s2,srl,5
  a30212:	8f89                	sub	a5,a5,a0
  a30214:	012a9023          	sh	s2,0(s5)
  a30218:	0685                	addi	a3,a3,1
  a3021a:	40a48533          	sub	a0,s1,a0
  a3021e:	b561                	j	a300a6 <LzmaDec_DecodeReal2+0xb04>
  a30220:	4b29191b          	subshf	s2,s2,s2,srl,5
  a30224:	8f85                	sub	a5,a5,s1
  a30226:	012a9023          	sh	s2,0(s5)
  a3022a:	0685                	addi	a3,a3,1
  a3022c:	409504b3          	sub	s1,a0,s1
  a30230:	b55d                	j	a300d6 <LzmaDec_DecodeReal2+0xb34>
  a30232:	4b29191b          	subshf	s2,s2,s2,srl,5
  a30236:	8f89                	sub	a5,a5,a0
  a30238:	012a9023          	sh	s2,0(s5)
  a3023c:	0685                	addi	a3,a3,1
  a3023e:	40a48533          	sub	a0,s1,a0
  a30242:	b5d1                	j	a30106 <LzmaDec_DecodeReal2+0xb64>
  a30244:	4b29191b          	subshf	s2,s2,s2,srl,5
  a30248:	8f85                	sub	a5,a5,s1
  a3024a:	012a9023          	sh	s2,0(s5)
  a3024e:	0685                	addi	a3,a3,1
  a30250:	409504b3          	sub	s1,a0,s1
  a30254:	b5cd                	j	a30136 <LzmaDec_DecodeReal2+0xb94>
  a30256:	4aa5151b          	subshf	a0,a0,a0,srl,5
  a3025a:	412787b3          	sub	a5,a5,s2
  a3025e:	00a31023          	sh	a0,0(t1)
  a30262:	0685                	addi	a3,a3,1
  a30264:	41248933          	sub	s2,s1,s2
  a30268:	bdfd                	j	a30166 <LzmaDec_DecodeReal2+0xbc4>
  a3026a:	4a94949b          	subshf	s1,s1,s1,srl,5
  a3026e:	41590933          	sub	s2,s2,s5
  a30272:	415787b3          	sub	a5,a5,s5
  a30276:	009b1023          	sh	s1,0(s6)
  a3027a:	9536                	add	a0,a0,a3
  a3027c:	b7b9                	j	a301ca <LzmaDec_DecodeReal2+0xc28>
  a3027e:	136d                	addi	t1,t1,-5
  a30280:	00c97763          	bgeu	s2,a2,a3028e <LzmaDec_DecodeReal2+0xcec>
  a30284:	2314                	lbu	a3,0(a4)
  a30286:	0922                	slli	s2,s2,0x8
  a30288:	0705                	addi	a4,a4,1
  a3028a:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a3028e:	00195913          	srli	s2,s2,0x1
  a30292:	412787b3          	sub	a5,a5,s2
  a30296:	41f7d493          	srai	s1,a5,0x1f
  a3029a:	02a4851b          	addshf	a0,s1,a0,sll,1
  a3029e:	137d                	addi	t1,t1,-1
  a302a0:	009974b3          	and	s1,s2,s1
  a302a4:	0505                	addi	a0,a0,1
  a302a6:	97a6                	add	a5,a5,s1
  a302a8:	fc031ce3          	bnez	t1,a30280 <LzmaDec_DecodeReal2+0xcde>
  a302ac:	0512                	slli	a0,a0,0x4
  a302ae:	002fd683          	lhu	a3,2(t6)
  a302b2:	00c97863          	bgeu	s2,a2,a302c2 <LzmaDec_DecodeReal2+0xd20>
  a302b6:	00074303          	lbu	t1,0(a4)
  a302ba:	0922                	slli	s2,s2,0x8
  a302bc:	0705                	addi	a4,a4,1
  a302be:	10f3279b          	orshf	a5,t1,a5,sll,8
  a302c2:	00b95493          	srli	s1,s2,0xb
  a302c6:	02d484b3          	mul	s1,s1,a3
  a302ca:	1097fc63          	bgeu	a5,s1,a303e2 <LzmaDec_DecodeReal2+0xe40>
  a302ce:	40d80333          	sub	t1,a6,a3
  a302d2:	4a66869b          	addshf	a3,a3,t1,srl,5
  a302d6:	00df9123          	sh	a3,2(t6)
  a302da:	4689                	li	a3,2
  a302dc:	02df8a9b          	addshf	s5,t6,a3,sll,1
  a302e0:	000ad903          	lhu	s2,0(s5)
  a302e4:	00c4f863          	bgeu	s1,a2,a302f4 <LzmaDec_DecodeReal2+0xd52>
  a302e8:	00074303          	lbu	t1,0(a4)
  a302ec:	04a2                	slli	s1,s1,0x8
  a302ee:	0705                	addi	a4,a4,1
  a302f0:	10f3279b          	orshf	a5,t1,a5,sll,8
  a302f4:	00b4d313          	srli	t1,s1,0xb
  a302f8:	03230333          	mul	t1,t1,s2
  a302fc:	0e67fc63          	bgeu	a5,t1,a303f4 <LzmaDec_DecodeReal2+0xe52>
  a30300:	412804b3          	sub	s1,a6,s2
  a30304:	4a99091b          	addshf	s2,s2,s1,srl,5
  a30308:	012a9023          	sh	s2,0(s5)
  a3030c:	0689                	addi	a3,a3,2
  a3030e:	02df8a9b          	addshf	s5,t6,a3,sll,1
  a30312:	000ad903          	lhu	s2,0(s5)
  a30316:	00c37763          	bgeu	t1,a2,a30324 <LzmaDec_DecodeReal2+0xd82>
  a3031a:	2304                	lbu	s1,0(a4)
  a3031c:	0322                	slli	t1,t1,0x8
  a3031e:	0705                	addi	a4,a4,1
  a30320:	10f4a79b          	orshf	a5,s1,a5,sll,8
  a30324:	00b35493          	srli	s1,t1,0xb
  a30328:	032484b3          	mul	s1,s1,s2
  a3032c:	0c97fe63          	bgeu	a5,s1,a30408 <LzmaDec_DecodeReal2+0xe66>
  a30330:	41280333          	sub	t1,a6,s2
  a30334:	4a69091b          	addshf	s2,s2,t1,srl,5
  a30338:	012a9023          	sh	s2,0(s5)
  a3033c:	0691                	addi	a3,a3,4
  a3033e:	02df8a9b          	addshf	s5,t6,a3,sll,1
  a30342:	000ad303          	lhu	t1,0(s5)
  a30346:	00c4f863          	bgeu	s1,a2,a30356 <LzmaDec_DecodeReal2+0xdb4>
  a3034a:	00074903          	lbu	s2,0(a4)
  a3034e:	04a2                	slli	s1,s1,0x8
  a30350:	0705                	addi	a4,a4,1
  a30352:	10f9279b          	orshf	a5,s2,a5,sll,8
  a30356:	00b4d913          	srli	s2,s1,0xb
  a3035a:	02690933          	mul	s2,s2,t1
  a3035e:	0b27fe63          	bgeu	a5,s2,a3041a <LzmaDec_DecodeReal2+0xe78>
  a30362:	406804b3          	sub	s1,a6,t1
  a30366:	4a93031b          	addshf	t1,t1,s1,srl,5
  a3036a:	006a9023          	sh	t1,0(s5)
  a3036e:	16e1                	addi	a3,a3,-8
  a30370:	8ec9                	or	a3,a3,a0
  a30372:	557d                	li	a0,-1
  a30374:	e6a691e3          	bne	a3,a0,a301d6 <LzmaDec_DecodeReal2+0xc34>
  a30378:	1f51                	addi	t5,t5,-12
  a3037a:	11200893          	li	a7,274
  a3037e:	010006b7          	lui	a3,0x1000
  a30382:	00d97763          	bgeu	s2,a3,a30390 <LzmaDec_DecodeReal2+0xdee>
  a30386:	2314                	lbu	a3,0(a4)
  a30388:	0922                	slli	s2,s2,0x8
  a3038a:	0705                	addi	a4,a4,1
  a3038c:	10f6a79b          	orshf	a5,a3,a5,sll,8
  a30390:	02fe2223          	sw	a5,36(t3)
  a30394:	02ce2783          	lw	a5,44(t3)
  a30398:	2008b513          	sltiu	a0,a7,512
  a3039c:	00ee2e23          	sw	a4,28(t3)
  a303a0:	032e2023          	sw	s2,32(t3)
  a303a4:	051e2223          	sw	a7,68(t3)
  a303a8:	01de2c23          	sw	t4,24(t3)
  a303ac:	027e2423          	sw	t2,40(t3)
  a303b0:	025e2823          	sw	t0,48(t3)
  a303b4:	028e2a23          	sw	s0,52(t3)
  a303b8:	033e2c23          	sw	s3,56(t3)
  a303bc:	03ae2e23          	sw	s10,60(t3)
  a303c0:	05ee2023          	sw	t5,64(t3)
  a303c4:	00154513          	xori	a0,a0,1
  a303c8:	e799                	bnez	a5,a303d6 <LzmaDec_DecodeReal2+0xe34>
  a303ca:	004e2783          	lw	a5,4(t3)
  a303ce:	00f3e463          	bltu	t2,a5,a303d6 <LzmaDec_DecodeReal2+0xe34>
  a303d2:	02fe2623          	sw	a5,44(t3)
  a303d6:	03010293          	addi	t0,sp,48
  a303da:	7fe2860b          	ldmia	{s0-s11},(t0)
  a303de:	6125                	addi	sp,sp,96
  a303e0:	8082                	ret
  a303e2:	4ad6969b          	subshf	a3,a3,a3,srl,5
  a303e6:	8f85                	sub	a5,a5,s1
  a303e8:	00df9123          	sh	a3,2(t6)
  a303ec:	409904b3          	sub	s1,s2,s1
  a303f0:	468d                	li	a3,3
  a303f2:	b5ed                	j	a302dc <LzmaDec_DecodeReal2+0xd3a>
  a303f4:	4b29191b          	subshf	s2,s2,s2,srl,5
  a303f8:	406787b3          	sub	a5,a5,t1
  a303fc:	012a9023          	sh	s2,0(s5)
  a30400:	0691                	addi	a3,a3,4 # 1000004 <_gp_+0x5cb848>
  a30402:	40648333          	sub	t1,s1,t1
  a30406:	b721                	j	a3030e <LzmaDec_DecodeReal2+0xd6c>
  a30408:	4b29191b          	subshf	s2,s2,s2,srl,5
  a3040c:	8f85                	sub	a5,a5,s1
  a3040e:	012a9023          	sh	s2,0(s5)
  a30412:	06a1                	addi	a3,a3,8
  a30414:	409304b3          	sub	s1,t1,s1
  a30418:	b71d                	j	a3033e <LzmaDec_DecodeReal2+0xd9c>
  a3041a:	4a63131b          	subshf	t1,t1,t1,srl,5
  a3041e:	412787b3          	sub	a5,a5,s2
  a30422:	006a9023          	sh	t1,0(s5)
  a30426:	41248933          	sub	s2,s1,s2
  a3042a:	b799                	j	a30370 <LzmaDec_DecodeReal2+0xdce>
  a3042c:	4f29                	li	t5,10
  a3042e:	bb55                	j	a301e2 <LzmaDec_DecodeReal2+0xc40>
  a30430:	8d4e                	mv	s10,s3
  a30432:	89a2                	mv	s3,s0
  a30434:	8416                	mv	s0,t0
  a30436:	82aa                	mv	t0,a0
  a30438:	b845                	j	a2fce8 <LzmaDec_DecodeReal2+0x746>
  a3043a:	00da0533          	add	a0,s4,a3
  a3043e:	00054303          	lbu	t1,0(a0)
  a30442:	0e85                	addi	t4,t4,1
  a30444:	01da0533          	add	a0,s4,t4
  a30448:	fe650fa3          	sb	t1,-1(a0)
  a3044c:	0685                	addi	a3,a3,1
  a3044e:	00dc9363          	bne	s9,a3,a30454 <LzmaDec_DecodeReal2+0xeb2>
  a30452:	4681                	li	a3,0
  a30454:	fe9e93e3          	bne	t4,s1,a3043a <LzmaDec_DecodeReal2+0xe98>
  a30458:	c14ff06f          	j	a2f86c <LzmaDec_DecodeReal2+0x2ca>
  a3045c:	4f2d                	li	t5,11
  a3045e:	c0eff06f          	j	a2f86c <LzmaDec_DecodeReal2+0x2ca>

00a30462 <LzmaDec_Init>:
  a30462:	11400793          	li	a5,276
  a30466:	00052c23          	sw	zero,24(a0)
  a3046a:	04052623          	sw	zero,76(a0)
  a3046e:	02052423          	sw	zero,40(a0)
  a30472:	02052623          	sw	zero,44(a0)
  a30476:	c17c                	sw	a5,68(a0)
  a30478:	8082                	ret

00a3047a <LzmaDec_DecodeToDic>:
  a3047a:	82d8                	push	{ra,s0-s11},-96
  a3047c:	0006a983          	lw	s3,0(a3)
  a30480:	0006a023          	sw	zero,0(a3)
  a30484:	00078023          	sb	zero,0(a5) # 1000000 <_gp_+0x5cb844>
  a30488:	8dbe                	mv	s11,a5
  a3048a:	417c                	lw	a5,68(a0)
  a3048c:	8bba                	mv	s7,a4
  a3048e:	11200713          	li	a4,274
  a30492:	842a                	mv	s0,a0
  a30494:	8b2e                	mv	s6,a1
  a30496:	84b2                	mv	s1,a2
  a30498:	8936                	mv	s2,a3
  a3049a:	08f77a63          	bgeu	a4,a5,a3052e <LzmaDec_DecodeToDic+0xb4>
  a3049e:	11400713          	li	a4,276
  a304a2:	0af76663          	bltu	a4,a5,a3054e <LzmaDec_DecodeToDic+0xd4>
  a304a6:	4711                	li	a4,4
  a304a8:	447c                	lw	a5,76(s0)
  a304aa:	0c098963          	beqz	s3,a3057c <LzmaDec_DecodeToDic+0x102>
  a304ae:	0af77763          	bgeu	a4,a5,a3055c <LzmaDec_DecodeToDic+0xe2>
  a304b2:	05044703          	lbu	a4,80(s0)
  a304b6:	e34d                	bnez	a4,a30558 <LzmaDec_DecodeToDic+0xde>
  a304b8:	0537e1bb          	bltui	a5,5,a3057e <LzmaDec_DecodeToDic+0x104>
  a304bc:	05244783          	lbu	a5,82(s0)
  a304c0:	05144703          	lbu	a4,81(s0)
  a304c4:	07c2                	slli	a5,a5,0x10
  a304c6:	30e7a79b          	orshf	a5,a5,a4,sll,24
  a304ca:	05444703          	lbu	a4,84(s0)
  a304ce:	8fd9                	or	a5,a5,a4
  a304d0:	05344703          	lbu	a4,83(s0)
  a304d4:	10e7a79b          	orshf	a5,a5,a4,sll,8
  a304d8:	5458                	lw	a4,44(s0)
  a304da:	d05c                	sw	a5,36(s0)
  a304dc:	eb01                	bnez	a4,a304ec <LzmaDec_DecodeToDic+0x72>
  a304de:	5418                	lw	a4,40(s0)
  a304e0:	e711                	bnez	a4,a304ec <LzmaDec_DecodeToDic+0x72>
  a304e2:	bfff fbff 071f      	l.li	a4,0xbffffbff
  a304e8:	06f76863          	bltu	a4,a5,a30558 <LzmaDec_DecodeToDic+0xde>
  a304ec:	4078                	lw	a4,68(s0)
  a304ee:	57fd                	li	a5,-1
  a304f0:	d01c                	sw	a5,32(s0)
  a304f2:	04042623          	sw	zero,76(s0)
  a304f6:	11300793          	li	a5,275
  a304fa:	02e7f863          	bgeu	a5,a4,a3052a <LzmaDec_DecodeToDic+0xb0>
  a304fe:	201c                	lbu	a5,0(s0)
  a30500:	3018                	lbu	a4,1(s0)
  a30502:	4414                	lw	a3,8(s0)
  a30504:	40000613          	li	a2,1024
  a30508:	973e                	add	a4,a4,a5
  a3050a:	30000793          	li	a5,768
  a3050e:	00e797b3          	sll	a5,a5,a4
  a30512:	7c078793          	addi	a5,a5,1984
  a30516:	4701                	li	a4,0
  a30518:	06f76563          	bltu	a4,a5,a30582 <LzmaDec_DecodeToDic+0x108>
  a3051c:	4785                	li	a5,1
  a3051e:	dc5c                	sw	a5,60(s0)
  a30520:	dc1c                	sw	a5,56(s0)
  a30522:	d85c                	sw	a5,52(s0)
  a30524:	d81c                	sw	a5,48(s0)
  a30526:	04042023          	sw	zero,64(s0)
  a3052a:	04042223          	sw	zero,68(s0)
  a3052e:	4c4d                	li	s8,19
  a30530:	5d7d                	li	s10,-1
  a30532:	05040a93          	addi	s5,s0,80
  a30536:	4074                	lw	a3,68(s0)
  a30538:	11200793          	li	a5,274
  a3053c:	04f69b63          	bne	a3,a5,a30592 <LzmaDec_DecodeToDic+0x118>
  a30540:	505c                	lw	a5,36(s0)
  a30542:	eb99                	bnez	a5,a30558 <LzmaDec_DecodeToDic+0xde>
  a30544:	4785                	li	a5,1
  a30546:	00fd8023          	sb	a5,0(s11)
  a3054a:	4501                	li	a0,0
  a3054c:	82d4                	popret	{ra,s0-s11},96
  a3054e:	1ff00713          	li	a4,511
  a30552:	452d                	li	a0,11
  a30554:	fee78ce3          	beq	a5,a4,a3054c <LzmaDec_DecodeToDic+0xd2>
  a30558:	4505                	li	a0,1
  a3055a:	bfcd                	j	a3054c <LzmaDec_DecodeToDic+0xd2>
  a3055c:	00178693          	addi	a3,a5,1
  a30560:	c474                	sw	a3,76(s0)
  a30562:	0485                	addi	s1,s1,1
  a30564:	fff4c683          	lbu	a3,-1(s1)
  a30568:	97a2                	add	a5,a5,s0
  a3056a:	19fd                	addi	s3,s3,-1
  a3056c:	04d78823          	sb	a3,80(a5)
  a30570:	00092783          	lw	a5,0(s2)
  a30574:	0785                	addi	a5,a5,1
  a30576:	00f92023          	sw	a5,0(s2)
  a3057a:	b73d                	j	a304a8 <LzmaDec_DecodeToDic+0x2e>
  a3057c:	fb9d                	bnez	a5,a304b2 <LzmaDec_DecodeToDic+0x38>
  a3057e:	478d                	li	a5,3
  a30580:	b7d9                	j	a30546 <LzmaDec_DecodeToDic+0xcc>
  a30582:	a292                	sh	a2,0(a3)
  a30584:	0705                	addi	a4,a4,1
  a30586:	0689                	addi	a3,a3,2
  a30588:	bf41                	j	a30518 <LzmaDec_DecodeToDic+0x9e>
  a3058a:	409989b3          	sub	s3,s3,s1
  a3058e:	84b6                	mv	s1,a3
  a30590:	b75d                	j	a30536 <LzmaDec_DecodeToDic+0xbc>
  a30592:	c699                	beqz	a3,a305a0 <LzmaDec_DecodeToDic+0x126>
  a30594:	4c1c                	lw	a5,24(s0)
  a30596:	40fb0733          	sub	a4,s6,a5
  a3059a:	00d77d63          	bgeu	a4,a3,a305b4 <LzmaDec_DecodeToDic+0x13a>
  a3059e:	ef01                	bnez	a4,a305b6 <LzmaDec_DecodeToDic+0x13c>
  a305a0:	4c1c                	lw	a5,24(s0)
  a305a2:	0767e163          	bltu	a5,s6,a30604 <LzmaDec_DecodeToDic+0x18a>
  a305a6:	407c                	lw	a5,68(s0)
  a305a8:	1a079a63          	bnez	a5,a3075c <LzmaDec_DecodeToDic+0x2e2>
  a305ac:	505c                	lw	a5,36(s0)
  a305ae:	e7b9                	bnez	a5,a305fc <LzmaDec_DecodeToDic+0x182>
  a305b0:	4791                	li	a5,4
  a305b2:	bf51                	j	a30546 <LzmaDec_DecodeToDic+0xcc>
  a305b4:	8736                	mv	a4,a3
  a305b6:	544c                	lw	a1,44(s0)
  a305b8:	5410                	lw	a2,40(s0)
  a305ba:	e599                	bnez	a1,a305c8 <LzmaDec_DecodeToDic+0x14e>
  a305bc:	404c                	lw	a1,4(s0)
  a305be:	40c58533          	sub	a0,a1,a2
  a305c2:	00a76363          	bltu	a4,a0,a305c8 <LzmaDec_DecodeToDic+0x14e>
  a305c6:	d44c                	sw	a1,44(s0)
  a305c8:	963a                	add	a2,a2,a4
  a305ca:	d410                	sw	a2,40(s0)
  a305cc:	580c                	lw	a1,48(s0)
  a305ce:	4810                	lw	a2,16(s0)
  a305d0:	01442803          	lw	a6,20(s0)
  a305d4:	8e99                	sub	a3,a3,a4
  a305d6:	c074                	sw	a3,68(s0)
  a305d8:	973e                	add	a4,a4,a5
  a305da:	40b78533          	sub	a0,a5,a1
  a305de:	4681                	li	a3,0
  a305e0:	00b7f363          	bgeu	a5,a1,a305e6 <LzmaDec_DecodeToDic+0x16c>
  a305e4:	86c2                	mv	a3,a6
  a305e6:	96b2                	add	a3,a3,a2
  a305e8:	96aa                	add	a3,a3,a0
  a305ea:	2288                	lbu	a0,0(a3)
  a305ec:	00f606b3          	add	a3,a2,a5
  a305f0:	0785                	addi	a5,a5,1
  a305f2:	a288                	sb	a0,0(a3)
  a305f4:	fee793e3          	bne	a5,a4,a305da <LzmaDec_DecodeToDic+0x160>
  a305f8:	cc1c                	sw	a5,24(s0)
  a305fa:	b75d                	j	a305a0 <LzmaDec_DecodeToDic+0x126>
  a305fc:	040b9863          	bnez	s7,a3064c <LzmaDec_DecodeToDic+0x1d2>
  a30600:	4789                	li	a5,2
  a30602:	b791                	j	a30546 <LzmaDec_DecodeToDic+0xcc>
  a30604:	4c81                	li	s9,0
  a30606:	4478                	lw	a4,76(s0)
  a30608:	10071f63          	bnez	a4,a30726 <LzmaDec_DecodeToDic+0x2ac>
  a3060c:	013c7463          	bgeu	s8,s3,a30614 <LzmaDec_DecodeToDic+0x19a>
  a30610:	0a0c8163          	beqz	s9,a306b2 <LzmaDec_DecodeToDic+0x238>
  a30614:	01348a33          	add	s4,s1,s3
  a30618:	0870                	addi	a2,sp,28
  a3061a:	85a6                	mv	a1,s1
  a3061c:	8522                	mv	a0,s0
  a3061e:	ce52                	sw	s4,28(sp)
  a30620:	b2bfe0ef          	jal	ra,a2f14a <LzmaDec_TryDummy>
  a30624:	e515                	bnez	a0,a30650 <LzmaDec_DecodeToDic+0x1d6>
  a30626:	1429f0bb          	bgeui	s3,20,a306a8 <LzmaDec_DecodeToDic+0x22e>
  a3062a:	00092783          	lw	a5,0(s2)
  a3062e:	05040413          	addi	s0,s0,80
  a30632:	97ce                	add	a5,a5,s3
  a30634:	00f92023          	sw	a5,0(s2)
  a30638:	ff342e23          	sw	s3,-4(s0)
  a3063c:	f49a01e3          	beq	s4,s1,a3057e <LzmaDec_DecodeToDic+0x104>
  a30640:	209c                	lbu	a5,0(s1)
  a30642:	0405                	addi	s0,s0,1
  a30644:	0485                	addi	s1,s1,1
  a30646:	fef40fa3          	sb	a5,-1(s0)
  a3064a:	bfcd                	j	a3063c <LzmaDec_DecodeToDic+0x1c2>
  a3064c:	4c85                	li	s9,1
  a3064e:	bf65                	j	a30606 <LzmaDec_DecodeToDic+0x18c>
  a30650:	4a72                	lw	s4,28(sp)
  a30652:	409a0a33          	sub	s4,s4,s1
  a30656:	151a74bb          	bgeui	s4,21,a306a8 <LzmaDec_DecodeToDic+0x22e>
  a3065a:	020c8a63          	beqz	s9,a3068e <LzmaDec_DecodeToDic+0x214>
  a3065e:	02050c3b          	beqi	a0,2,a3068e <LzmaDec_DecodeToDic+0x214>
  a30662:	00092783          	lw	a5,0(s2)
  a30666:	05040413          	addi	s0,s0,80
  a3066a:	97d2                	add	a5,a5,s4
  a3066c:	00f92023          	sw	a5,0(s2)
  a30670:	ff442e23          	sw	s4,-4(s0)
  a30674:	9a26                	add	s4,s4,s1
  a30676:	01449663          	bne	s1,s4,a30682 <LzmaDec_DecodeToDic+0x208>
  a3067a:	4789                	li	a5,2
  a3067c:	00fd8023          	sb	a5,0(s11)
  a30680:	bde1                	j	a30558 <LzmaDec_DecodeToDic+0xde>
  a30682:	209c                	lbu	a5,0(s1)
  a30684:	0405                	addi	s0,s0,1
  a30686:	0485                	addi	s1,s1,1
  a30688:	fef40fa3          	sb	a5,-1(s0)
  a3068c:	b7ed                	j	a30676 <LzmaDec_DecodeToDic+0x1fc>
  a3068e:	8626                	mv	a2,s1
  a30690:	cc44                	sw	s1,28(s0)
  a30692:	85da                	mv	a1,s6
  a30694:	8522                	mv	a0,s0
  a30696:	f0dfe0ef          	jal	ra,a2f5a2 <LzmaDec_DecodeReal2>
  a3069a:	4c54                	lw	a3,28(s0)
  a3069c:	409684b3          	sub	s1,a3,s1
  a306a0:	01aa1e63          	bne	s4,s10,a306bc <LzmaDec_DecodeToDic+0x242>
  a306a4:	0099fe63          	bgeu	s3,s1,a306c0 <LzmaDec_DecodeToDic+0x246>
  a306a8:	1ff00793          	li	a5,511
  a306ac:	c07c                	sw	a5,68(s0)
  a306ae:	452d                	li	a0,11
  a306b0:	bd71                	j	a3054c <LzmaDec_DecodeToDic+0xd2>
  a306b2:	fec98613          	addi	a2,s3,-20
  a306b6:	9626                	add	a2,a2,s1
  a306b8:	5a7d                	li	s4,-1
  a306ba:	bfd9                	j	a30690 <LzmaDec_DecodeToDic+0x216>
  a306bc:	ff4496e3          	bne	s1,s4,a306a8 <LzmaDec_DecodeToDic+0x22e>
  a306c0:	00092783          	lw	a5,0(s2)
  a306c4:	97a6                	add	a5,a5,s1
  a306c6:	00f92023          	sw	a5,0(s2)
  a306ca:	ec0500e3          	beqz	a0,a3058a <LzmaDec_DecodeToDic+0x110>
  a306ce:	20000793          	li	a5,512
  a306d2:	c07c                	sw	a5,68(s0)
  a306d4:	b551                	j	a30558 <LzmaDec_DecodeToDic+0xde>
  a306d6:	0a05                	addi	s4,s4,1
  a306d8:	40ea06b3          	sub	a3,s4,a4
  a306dc:	96a6                	add	a3,a3,s1
  a306de:	fff6c603          	lbu	a2,-1(a3)
  a306e2:	014406b3          	add	a3,s0,s4
  a306e6:	04c687a3          	sb	a2,79(a3)
  a306ea:	40ea06b3          	sub	a3,s4,a4
  a306ee:	074c6a63          	bltu	s8,s4,a30762 <LzmaDec_DecodeToDic+0x2e8>
  a306f2:	ff36e2e3          	bltu	a3,s3,a306d6 <LzmaDec_DecodeToDic+0x25c>
  a306f6:	014a8733          	add	a4,s5,s4
  a306fa:	0870                	addi	a2,sp,28
  a306fc:	85d6                	mv	a1,s5
  a306fe:	8522                	mv	a0,s0
  a30700:	c636                	sw	a3,12(sp)
  a30702:	ce3a                	sw	a4,28(sp)
  a30704:	a47fe0ef          	jal	ra,a2f14a <LzmaDec_TryDummy>
  a30708:	46b2                	lw	a3,12(sp)
  a3070a:	e105                	bnez	a0,a3072a <LzmaDec_DecodeToDic+0x2b0>
  a3070c:	14ea773b          	bgeui	s4,20,a306a8 <LzmaDec_DecodeToDic+0x22e>
  a30710:	05442623          	sw	s4,76(s0)
  a30714:	00092783          	lw	a5,0(s2)
  a30718:	97b6                	add	a5,a5,a3
  a3071a:	00f92023          	sw	a5,0(s2)
  a3071e:	478d                	li	a5,3
  a30720:	00fd8023          	sb	a5,0(s11)
  a30724:	b525                	j	a3054c <LzmaDec_DecodeToDic+0xd2>
  a30726:	8a3a                	mv	s4,a4
  a30728:	b7c9                	j	a306ea <LzmaDec_DecodeToDic+0x270>
  a3072a:	4a72                	lw	s4,28(sp)
  a3072c:	4474                	lw	a3,76(s0)
  a3072e:	415a0a33          	sub	s4,s4,s5
  a30732:	f6da6be3          	bltu	s4,a3,a306a8 <LzmaDec_DecodeToDic+0x22e>
  a30736:	020c8963          	beqz	s9,a30768 <LzmaDec_DecodeToDic+0x2ee>
  a3073a:	4709                	li	a4,2
  a3073c:	02e50663          	beq	a0,a4,a30768 <LzmaDec_DecodeToDic+0x2ee>
  a30740:	00092783          	lw	a5,0(s2)
  a30744:	97d2                	add	a5,a5,s4
  a30746:	8f95                	sub	a5,a5,a3
  a30748:	00f92023          	sw	a5,0(s2)
  a3074c:	05442623          	sw	s4,76(s0)
  a30750:	00ed8023          	sb	a4,0(s11)
  a30754:	b511                	j	a30558 <LzmaDec_DecodeToDic+0xde>
  a30756:	03478a63          	beq	a5,s4,a3078a <LzmaDec_DecodeToDic+0x310>
  a3075a:	b7b9                	j	a306a8 <LzmaDec_DecodeToDic+0x22e>
  a3075c:	ea0b82e3          	beqz	s7,a30600 <LzmaDec_DecodeToDic+0x186>
  a30760:	bf29                	j	a3067a <LzmaDec_DecodeToDic+0x200>
  a30762:	f80c9ae3          	bnez	s9,a306f6 <LzmaDec_DecodeToDic+0x27c>
  a30766:	5a7d                	li	s4,-1
  a30768:	01542e23          	sw	s5,28(s0)
  a3076c:	8656                	mv	a2,s5
  a3076e:	85da                	mv	a1,s6
  a30770:	8522                	mv	a0,s0
  a30772:	e31fe0ef          	jal	ra,a2f5a2 <LzmaDec_DecodeReal2>
  a30776:	4c5c                	lw	a5,28(s0)
  a30778:	4478                	lw	a4,76(s0)
  a3077a:	415787b3          	sub	a5,a5,s5
  a3077e:	fc0a5ce3          	bgez	s4,a30756 <LzmaDec_DecodeToDic+0x2dc>
  a30782:	15c7f9bb          	bgeui	a5,21,a306a8 <LzmaDec_DecodeToDic+0x22e>
  a30786:	f2e7e1e3          	bltu	a5,a4,a306a8 <LzmaDec_DecodeToDic+0x22e>
  a3078a:	8f99                	sub	a5,a5,a4
  a3078c:	00092703          	lw	a4,0(s2)
  a30790:	00f486b3          	add	a3,s1,a5
  a30794:	40f989b3          	sub	s3,s3,a5
  a30798:	97ba                	add	a5,a5,a4
  a3079a:	00f92023          	sw	a5,0(s2)
  a3079e:	04042623          	sw	zero,76(s0)
  a307a2:	de0506e3          	beqz	a0,a3058e <LzmaDec_DecodeToDic+0x114>
  a307a6:	b725                	j	a306ce <LzmaDec_DecodeToDic+0x254>

00a307a8 <LzmaDec_DecodeToBuf>:
  a307a8:	82c8                	push	{ra,s0-s10},-80
  a307aa:	00062903          	lw	s2,0(a2) # 1000000 <_gp_+0x5cb844>
  a307ae:	00072b03          	lw	s6,0(a4)
  a307b2:	00062023          	sw	zero,0(a2)
  a307b6:	842a                	mv	s0,a0
  a307b8:	8bae                	mv	s7,a1
  a307ba:	89b2                	mv	s3,a2
  a307bc:	8c36                	mv	s8,a3
  a307be:	8a3a                	mv	s4,a4
  a307c0:	8d3e                	mv	s10,a5
  a307c2:	00072023          	sw	zero,0(a4)
  a307c6:	484c                	lw	a1,20(s0)
  a307c8:	4c1c                	lw	a5,24(s0)
  a307ca:	ce5a                	sw	s6,28(sp)
  a307cc:	00b79463          	bne	a5,a1,a307d4 <LzmaDec_DecodeToBuf+0x2c>
  a307d0:	00042c23          	sw	zero,24(s0)
  a307d4:	01842a83          	lw	s5,24(s0)
  a307d8:	4701                	li	a4,0
  a307da:	415587b3          	sub	a5,a1,s5
  a307de:	0127e563          	bltu	a5,s2,a307e8 <LzmaDec_DecodeToBuf+0x40>
  a307e2:	015905b3          	add	a1,s2,s5
  a307e6:	876a                	mv	a4,s10
  a307e8:	87c2                	mv	a5,a6
  a307ea:	8662                	mv	a2,s8
  a307ec:	0874                	addi	a3,sp,28
  a307ee:	8522                	mv	a0,s0
  a307f0:	c642                	sw	a6,12(sp)
  a307f2:	c89ff0ef          	jal	ra,a3047a <LzmaDec_DecodeToDic>
  a307f6:	4772                	lw	a4,28(sp)
  a307f8:	000a2783          	lw	a5,0(s4)
  a307fc:	8caa                	mv	s9,a0
  a307fe:	855e                	mv	a0,s7
  a30800:	97ba                	add	a5,a5,a4
  a30802:	00fa2023          	sw	a5,0(s4)
  a30806:	4c04                	lw	s1,24(s0)
  a30808:	480c                	lw	a1,16(s0)
  a3080a:	9c3a                	add	s8,s8,a4
  a3080c:	415484b3          	sub	s1,s1,s5
  a30810:	8626                	mv	a2,s1
  a30812:	95d6                	add	a1,a1,s5
  a30814:	40eb0b33          	sub	s6,s6,a4
  a30818:	a3af80ef          	jal	ra,a28a52 <memcpy>
  a3081c:	0009a783          	lw	a5,0(s3)
  a30820:	9ba6                	add	s7,s7,s1
  a30822:	40990933          	sub	s2,s2,s1
  a30826:	97a6                	add	a5,a5,s1
  a30828:	00f9a023          	sw	a5,0(s3)
  a3082c:	000c9663          	bnez	s9,a30838 <LzmaDec_DecodeToBuf+0x90>
  a30830:	c481                	beqz	s1,a30838 <LzmaDec_DecodeToBuf+0x90>
  a30832:	4832                	lw	a6,12(sp)
  a30834:	f80919e3          	bnez	s2,a307c6 <LzmaDec_DecodeToBuf+0x1e>
  a30838:	8566                	mv	a0,s9
  a3083a:	82c4                	popret	{ra,s0-s10},80

00a3083c <LzmaDec_FreeProbs>:
  a3083c:	8028                	push	{ra,s0},-16
  a3083e:	842a                	mv	s0,a0
  a30840:	41dc                	lw	a5,4(a1)
  a30842:	852e                	mv	a0,a1
  a30844:	440c                	lw	a1,8(s0)
  a30846:	9782                	jalr	a5
  a30848:	00042423          	sw	zero,8(s0)
  a3084c:	8024                	popret	{ra,s0},16

00a3084e <LzmaDec_AllocateProbs2.isra.2>:
  a3084e:	8048                	push	{ra,s0-s2},-16
  a30850:	451c                	lw	a5,8(a0)
  a30852:	962e                	add	a2,a2,a1
  a30854:	30000493          	li	s1,768
  a30858:	00c494b3          	sll	s1,s1,a2
  a3085c:	842a                	mv	s0,a0
  a3085e:	7c048493          	addi	s1,s1,1984
  a30862:	c789                	beqz	a5,a3086c <LzmaDec_AllocateProbs2.isra.2+0x1e>
  a30864:	453c                	lw	a5,72(a0)
  a30866:	4501                	li	a0,0
  a30868:	02f48863          	beq	s1,a5,a30898 <LzmaDec_AllocateProbs2.isra.2+0x4a>
  a3086c:	85b6                	mv	a1,a3
  a3086e:	8522                	mv	a0,s0
  a30870:	8936                	mv	s2,a3
  a30872:	fcbff0ef          	jal	ra,a3083c <LzmaDec_FreeProbs>
  a30876:	00092783          	lw	a5,0(s2)
  a3087a:	00149593          	slli	a1,s1,0x1
  a3087e:	854a                	mv	a0,s2
  a30880:	9782                	jalr	a5
  a30882:	87aa                	mv	a5,a0
  a30884:	c408                	sw	a0,8(s0)
  a30886:	4509                	li	a0,2
  a30888:	cb81                	beqz	a5,a30898 <LzmaDec_AllocateProbs2.isra.2+0x4a>
  a3088a:	0000 0d00 051f      	l.li	a0,0xd00
  a30890:	97aa                	add	a5,a5,a0
  a30892:	c45c                	sw	a5,12(s0)
  a30894:	c424                	sw	s1,72(s0)
  a30896:	4501                	li	a0,0
  a30898:	8044                	popret	{ra,s0-s2},16

00a3089a <LzmaProps_Decode>:
  a3089a:	0506723b          	bgeui	a2,5,a308a2 <LzmaProps_Decode+0x8>
  a3089e:	4511                	li	a0,4
  a308a0:	8082                	ret
  a308a2:	21b8                	lbu	a4,2(a1)
  a308a4:	319c                	lbu	a5,1(a1)
  a308a6:	10e7a79b          	orshf	a5,a5,a4,sll,8
  a308aa:	31b8                	lbu	a4,3(a1)
  a308ac:	20e7a79b          	orshf	a5,a5,a4,sll,16
  a308b0:	21d8                	lbu	a4,4(a1)
  a308b2:	30e7a79b          	orshf	a5,a5,a4,sll,24
  a308b6:	6705                	lui	a4,0x1
  a308b8:	00e7f363          	bgeu	a5,a4,a308be <LzmaProps_Decode+0x24>
  a308bc:	6785                	lui	a5,0x1
  a308be:	c15c                	sw	a5,4(a0)
  a308c0:	219c                	lbu	a5,0(a1)
  a308c2:	e1f7f73b          	bgeui	a5,225,a3089e <LzmaProps_Decode+0x4>
  a308c6:	46a5                	li	a3,9
  a308c8:	02d7f733          	remu	a4,a5,a3
  a308cc:	a118                	sb	a4,0(a0)
  a308ce:	02d00713          	li	a4,45
  a308d2:	02e7d733          	divu	a4,a5,a4
  a308d6:	02d7d7b3          	divu	a5,a5,a3
  a308da:	a138                	sb	a4,2(a0)
  a308dc:	4715                	li	a4,5
  a308de:	02e7f7b3          	remu	a5,a5,a4
  a308e2:	b11c                	sb	a5,1(a0)
  a308e4:	4501                	li	a0,0
  a308e6:	8082                	ret

00a308e8 <LzmaDec_AllocateProbs>:
  a308e8:	8228                	push	{ra,s0},-48
  a308ea:	842a                	mv	s0,a0
  a308ec:	0828                	addi	a0,sp,24
  a308ee:	c636                	sw	a3,12(sp)
  a308f0:	fabff0ef          	jal	ra,a3089a <LzmaProps_Decode>
  a308f4:	46b2                	lw	a3,12(sp)
  a308f6:	ed09                	bnez	a0,a30910 <LzmaDec_AllocateProbs+0x28>
  a308f8:	01914603          	lbu	a2,25(sp)
  a308fc:	01814583          	lbu	a1,24(sp)
  a30900:	8522                	mv	a0,s0
  a30902:	f4dff0ef          	jal	ra,a3084e <LzmaDec_AllocateProbs2.isra.2>
  a30906:	e509                	bnez	a0,a30910 <LzmaDec_AllocateProbs+0x28>
  a30908:	47e2                	lw	a5,24(sp)
  a3090a:	c01c                	sw	a5,0(s0)
  a3090c:	47f2                	lw	a5,28(sp)
  a3090e:	c05c                	sw	a5,4(s0)
  a30910:	8224                	popret	{ra,s0},48

00a30912 <SecMemcpyError>:
  a30912:	04b05b63          	blez	a1,a30968 <SecMemcpyError+0x56>
  a30916:	87aa                	mv	a5,a0
  a30918:	4859                	li	a6,22
  a3091a:	c929                	beqz	a0,a3096c <SecMemcpyError+0x5a>
  a3091c:	8018                	push	{ra},-16
  a3091e:	8732                	mv	a4,a2
  a30920:	862e                	mv	a2,a1
  a30922:	eb01                	bnez	a4,a30932 <SecMemcpyError+0x20>
  a30924:	4581                	li	a1,0
  a30926:	87ef80ef          	jal	ra,a289a4 <memset>
  a3092a:	09600813          	li	a6,150
  a3092e:	8542                	mv	a0,a6
  a30930:	8014                	popret	{ra},16
  a30932:	00d5f863          	bgeu	a1,a3,a30942 <SecMemcpyError+0x30>
  a30936:	4581                	li	a1,0
  a30938:	86cf80ef          	jal	ra,a289a4 <memset>
  a3093c:	0a200813          	li	a6,162
  a30940:	b7fd                	j	a3092e <SecMemcpyError+0x1c>
  a30942:	00a77663          	bgeu	a4,a0,a3094e <SecMemcpyError+0x3c>
  a30946:	00d705b3          	add	a1,a4,a3
  a3094a:	00b56863          	bltu	a0,a1,a3095a <SecMemcpyError+0x48>
  a3094e:	4801                	li	a6,0
  a30950:	fce7ffe3          	bgeu	a5,a4,a3092e <SecMemcpyError+0x1c>
  a30954:	96be                	add	a3,a3,a5
  a30956:	fcd77ce3          	bgeu	a4,a3,a3092e <SecMemcpyError+0x1c>
  a3095a:	4581                	li	a1,0
  a3095c:	853e                	mv	a0,a5
  a3095e:	846f80ef          	jal	ra,a289a4 <memset>
  a30962:	0b600813          	li	a6,182
  a30966:	b7e1                	j	a3092e <SecMemcpyError+0x1c>
  a30968:	02200813          	li	a6,34
  a3096c:	8542                	mv	a0,a6
  a3096e:	8082                	ret

00a30970 <memcpy_s>:
  a30970:	02d5e363          	bltu	a1,a3,a30996 <memcpy_s+0x26>
  a30974:	c10d                	beqz	a0,a30996 <memcpy_s+0x26>
  a30976:	c205                	beqz	a2,a30996 <memcpy_s+0x26>
  a30978:	0005cf63          	bltz	a1,a30996 <memcpy_s+0x26>
  a3097c:	ce89                	beqz	a3,a30996 <memcpy_s+0x26>
  a3097e:	00a67663          	bgeu	a2,a0,a3098a <memcpy_s+0x1a>
  a30982:	00d607b3          	add	a5,a2,a3
  a30986:	00f57a63          	bgeu	a0,a5,a3099a <memcpy_s+0x2a>
  a3098a:	00c57663          	bgeu	a0,a2,a30996 <memcpy_s+0x26>
  a3098e:	00d507b3          	add	a5,a0,a3
  a30992:	00f67463          	bgeu	a2,a5,a3099a <memcpy_s+0x2a>
  a30996:	f7dff06f          	j	a30912 <SecMemcpyError>
  a3099a:	8018                	push	{ra},-16
  a3099c:	85b2                	mv	a1,a2
  a3099e:	8636                	mv	a2,a3
  a309a0:	8b2f80ef          	jal	ra,a28a52 <memcpy>
  a309a4:	4501                	li	a0,0
  a309a6:	8014                	popret	{ra},16

00a309a8 <memmove_s>:
  a309a8:	8028                	push	{ra,s0},-16
  a309aa:	02200413          	li	s0,34
  a309ae:	00b05c63          	blez	a1,a309c6 <memmove_s+0x1e>
  a309b2:	4459                	li	s0,22
  a309b4:	c909                	beqz	a0,a309c6 <memmove_s+0x1e>
  a309b6:	87b2                	mv	a5,a2
  a309b8:	862e                	mv	a2,a1
  a309ba:	eb81                	bnez	a5,a309ca <memmove_s+0x22>
  a309bc:	4581                	li	a1,0
  a309be:	fe7f70ef          	jal	ra,a289a4 <memset>
  a309c2:	09600413          	li	s0,150
  a309c6:	8522                	mv	a0,s0
  a309c8:	8024                	popret	{ra,s0},16
  a309ca:	00d5f863          	bgeu	a1,a3,a309da <memmove_s+0x32>
  a309ce:	4581                	li	a1,0
  a309d0:	fd5f70ef          	jal	ra,a289a4 <memset>
  a309d4:	0a200413          	li	s0,162
  a309d8:	b7fd                	j	a309c6 <memmove_s+0x1e>
  a309da:	4401                	li	s0,0
  a309dc:	fef505e3          	beq	a0,a5,a309c6 <memmove_s+0x1e>
  a309e0:	d2fd                	beqz	a3,a309c6 <memmove_s+0x1e>
  a309e2:	8636                	mv	a2,a3
  a309e4:	85be                	mv	a1,a5
  a309e6:	b64f80ef          	jal	ra,a28d4a <memmove>
  a309ea:	bff1                	j	a309c6 <memmove_s+0x1e>

00a309ec <memset_s>:
  a309ec:	87ae                	mv	a5,a1
  a309ee:	0205c363          	bltz	a1,a30a14 <memset_s+0x28>
  a309f2:	8028                	push	{ra,s0},-16
  a309f4:	842a                	mv	s0,a0
  a309f6:	c911                	beqz	a0,a30a0a <memset_s+0x1e>
  a309f8:	85b2                	mv	a1,a2
  a309fa:	8636                	mv	a2,a3
  a309fc:	02d7e063          	bltu	a5,a3,a30a1c <memset_s+0x30>
  a30a00:	fa5f70ef          	jal	ra,a289a4 <memset>
  a30a04:	4701                	li	a4,0
  a30a06:	853a                	mv	a0,a4
  a30a08:	8024                	popret	{ra,s0},16
  a30a0a:	02200713          	li	a4,34
  a30a0e:	dde5                	beqz	a1,a30a06 <memset_s+0x1a>
  a30a10:	4759                	li	a4,22
  a30a12:	bfd5                	j	a30a06 <memset_s+0x1a>
  a30a14:	02200713          	li	a4,34
  a30a18:	853a                	mv	a0,a4
  a30a1a:	8082                	ret
  a30a1c:	02200713          	li	a4,34
  a30a20:	d3fd                	beqz	a5,a30a06 <memset_s+0x1a>
  a30a22:	863e                	mv	a2,a5
  a30a24:	f81f70ef          	jal	ra,a289a4 <memset>
  a30a28:	0a200713          	li	a4,162
  a30a2c:	bfe9                	j	a30a06 <memset_s+0x1a>

00a30a2e <SecWriteMultiChar>:
  a30a2e:	8732                	mv	a4,a2
  a30a30:	00e04663          	bgtz	a4,a30a3c <SecWriteMultiChar+0xe>
  a30a34:	429c                	lw	a5,0(a3)
  a30a36:	963e                	add	a2,a2,a5
  a30a38:	c290                	sw	a2,0(a3)
  a30a3a:	8082                	ret
  a30a3c:	411c                	lw	a5,0(a0)
  a30a3e:	17fd                	addi	a5,a5,-1 # fff <ccause+0x3d>
  a30a40:	c11c                	sw	a5,0(a0)
  a30a42:	0007d563          	bgez	a5,a30a4c <SecWriteMultiChar+0x1e>
  a30a46:	57fd                	li	a5,-1
  a30a48:	c29c                	sw	a5,0(a3)
  a30a4a:	8082                	ret
  a30a4c:	415c                	lw	a5,4(a0)
  a30a4e:	177d                	addi	a4,a4,-1 # fff <ccause+0x3d>
  a30a50:	a38c                	sb	a1,0(a5)
  a30a52:	415c                	lw	a5,4(a0)
  a30a54:	0785                	addi	a5,a5,1
  a30a56:	c15c                	sw	a5,4(a0)
  a30a58:	bfe1                	j	a30a30 <SecWriteMultiChar+0x2>

00a30a5a <SecOutput>:
  a30a5a:	9fd8                	push	{ra,s0-s11},-560
  a30a5c:	00a34ab7          	lui	s5,0xa34
  a30a60:	c6ca8793          	addi	a5,s5,-916 # a33c6c <g_itoaUpperDigits>
  a30a64:	711d                	addi	sp,sp,-96
  a30a66:	cc3e                	sw	a5,24(sp)
  a30a68:	00a34b37          	lui	s6,0xa34
  a30a6c:	47a9                	li	a5,10
  a30a6e:	8caa                	mv	s9,a0
  a30a70:	8432                	mv	s0,a2
  a30a72:	ce02                	sw	zero,28(sp)
  a30a74:	d002                	sw	zero,32(sp)
  a30a76:	d202                	sw	zero,36(sp)
  a30a78:	d43e                	sw	a5,40(sp)
  a30a7a:	d602                	sw	zero,44(sp)
  a30a7c:	d802                	sw	zero,48(sp)
  a30a7e:	da02                	sw	zero,52(sp)
  a30a80:	dc02                	sw	zero,56(sp)
  a30a82:	de02                	sw	zero,60(sp)
  a30a84:	c082                	sw	zero,64(sp)
  a30a86:	c282                	sw	zero,68(sp)
  a30a88:	04011423          	sh	zero,72(sp)
  a30a8c:	ca02                	sw	zero,20(sp)
  a30a8e:	4701                	li	a4,0
  a30a90:	c6ca8a93          	addi	s5,s5,-916
  a30a94:	00a3 3c80 0b9f      	l.li	s7,0xa33c80
  a30a9a:	c38b0b13          	addi	s6,s6,-968 # a33c38 <g_efuse_cfg+0x143c>
  a30a9e:	00a3 3f7c 0c1f      	l.li	s8,0xa33f7c
  a30aa4:	00a3 3c58 091f      	l.li	s2,0xa33c58
  a30aaa:	219c                	lbu	a5,0(a1)
  a30aac:	c781                	beqz	a5,a30ab4 <SecOutput+0x5a>
  a30aae:	46d2                	lw	a3,20(sp)
  a30ab0:	0006d963          	bgez	a3,a30ac2 <SecOutput+0x68>
  a30ab4:	c319                	beqz	a4,a30aba <SecOutput+0x60>
  a30ab6:	0737163b          	bnei	a4,7,a30b8e <SecOutput+0x134>
  a30aba:	4552                	lw	a0,20(sp)
  a30abc:	25010113          	addi	sp,sp,592
  a30ac0:	80d4                	popret	{ra,s0-s11},64
  a30ac2:	00fb86b3          	add	a3,s7,a5
  a30ac6:	2294                	lbu	a3,0(a3)
  a30ac8:	00158a13          	addi	s4,a1,1
  a30acc:	08dbd6db          	muliadd	a3,s7,a3,9
  a30ad0:	96ba                	add	a3,a3,a4
  a30ad2:	1006c983          	lbu	s3,256(a3)
  a30ad6:	0829fe3b          	bgeui	s3,8,a30b8e <SecOutput+0x134>
  a30ada:	053b069b          	addshf	a3,s6,s3,sll,2
  a30ade:	4294                	lw	a3,0(a3)
  a30ae0:	8682                	jr	a3
  a30ae2:	000ca703          	lw	a4,0(s9)
  a30ae6:	177d                	addi	a4,a4,-1
  a30ae8:	00eca023          	sw	a4,0(s9)
  a30aec:	02074063          	bltz	a4,a30b0c <SecOutput+0xb2>
  a30af0:	004ca703          	lw	a4,4(s9)
  a30af4:	a31c                	sb	a5,0(a4)
  a30af6:	004ca783          	lw	a5,4(s9)
  a30afa:	0785                	addi	a5,a5,1
  a30afc:	00fca223          	sw	a5,4(s9)
  a30b00:	47d2                	lw	a5,20(sp)
  a30b02:	0785                	addi	a5,a5,1
  a30b04:	ca3e                	sw	a5,20(sp)
  a30b06:	85d2                	mv	a1,s4
  a30b08:	874e                	mv	a4,s3
  a30b0a:	b745                	j	a30aaa <SecOutput+0x50>
  a30b0c:	57fd                	li	a5,-1
  a30b0e:	bfdd                	j	a30b04 <SecOutput+0xaa>
  a30b10:	57fd                	li	a5,-1
  a30b12:	c282                	sw	zero,68(sp)
  a30b14:	d002                	sw	zero,32(sp)
  a30b16:	d602                	sw	zero,44(sp)
  a30b18:	d802                	sw	zero,48(sp)
  a30b1a:	da3e                	sw	a5,52(sp)
  a30b1c:	d202                	sw	zero,36(sp)
  a30b1e:	dc02                	sw	zero,56(sp)
  a30b20:	de02                	sw	zero,60(sp)
  a30b22:	b7d5                	j	a30b06 <SecOutput+0xac>
  a30b24:	02b00713          	li	a4,43
  a30b28:	02e78963          	beq	a5,a4,a30b5a <SecOutput+0x100>
  a30b2c:	00f76a63          	bltu	a4,a5,a30b40 <SecOutput+0xe6>
  a30b30:	2007883b          	beqi	a5,32,a30b50 <SecOutput+0xf6>
  a30b34:	23f794bb          	bnei	a5,35,a30b06 <SecOutput+0xac>
  a30b38:	57b2                	lw	a5,44(sp)
  a30b3a:	0807e793          	ori	a5,a5,128
  a30b3e:	a821                	j	a30b56 <SecOutput+0xfc>
  a30b40:	2d0788bb          	beqi	a5,45,a30b62 <SecOutput+0x108>
  a30b44:	30f790bb          	bnei	a5,48,a30b06 <SecOutput+0xac>
  a30b48:	57b2                	lw	a5,44(sp)
  a30b4a:	0087e793          	ori	a5,a5,8
  a30b4e:	a021                	j	a30b56 <SecOutput+0xfc>
  a30b50:	57b2                	lw	a5,44(sp)
  a30b52:	0027e793          	ori	a5,a5,2
  a30b56:	d63e                	sw	a5,44(sp)
  a30b58:	b77d                	j	a30b06 <SecOutput+0xac>
  a30b5a:	57b2                	lw	a5,44(sp)
  a30b5c:	0017e793          	ori	a5,a5,1
  a30b60:	bfdd                	j	a30b56 <SecOutput+0xfc>
  a30b62:	57b2                	lw	a5,44(sp)
  a30b64:	0047e793          	ori	a5,a5,4
  a30b68:	b7fd                	j	a30b56 <SecOutput+0xfc>
  a30b6a:	2a0793bb          	bnei	a5,42,a30b78 <SecOutput+0x11e>
  a30b6e:	4014                	lw	a3,0(s0)
  a30b70:	0411                	addi	s0,s0,4
  a30b72:	d836                	sw	a3,48(sp)
  a30b74:	4685                	li	a3,1
  a30b76:	dc36                	sw	a3,56(sp)
  a30b78:	56e2                	lw	a3,56(sp)
  a30b7a:	e695                	bnez	a3,a30ba6 <SecOutput+0x14c>
  a30b7c:	030701bb          	beqi	a4,3,a30b82 <SecOutput+0x128>
  a30b80:	d802                	sw	zero,48(sp)
  a30b82:	56c2                	lw	a3,48(sp)
  a30b84:	0147 ae14 071f      	l.li	a4,0x147ae14
  a30b8a:	00d75463          	bge	a4,a3,a30b92 <SecOutput+0x138>
  a30b8e:	557d                	li	a0,-1
  a30b90:	b735                	j	a30abc <SecOutput+0x62>
  a30b92:	00269713          	slli	a4,a3,0x2
  a30b96:	fd078793          	addi	a5,a5,-48
  a30b9a:	9736                	add	a4,a4,a3
  a30b9c:	9f81                	uxtb	a5
  a30b9e:	02e7879b          	addshf	a5,a5,a4,sll,1
  a30ba2:	d83e                	sw	a5,48(sp)
  a30ba4:	b78d                	j	a30b06 <SecOutput+0xac>
  a30ba6:	57c2                	lw	a5,48(sp)
  a30ba8:	0007d963          	bgez	a5,a30bba <SecOutput+0x160>
  a30bac:	5732                	lw	a4,44(sp)
  a30bae:	40f007b3          	neg	a5,a5
  a30bb2:	d83e                	sw	a5,48(sp)
  a30bb4:	00476713          	ori	a4,a4,4
  a30bb8:	d63a                	sw	a4,44(sp)
  a30bba:	5742                	lw	a4,48(sp)
  a30bbc:	0ccc ccc8 079f      	l.li	a5,0xcccccc8
  a30bc2:	f4e7d2e3          	bge	a5,a4,a30b06 <SecOutput+0xac>
  a30bc6:	b7e1                	j	a30b8e <SecOutput+0x134>
  a30bc8:	da02                	sw	zero,52(sp)
  a30bca:	bf35                	j	a30b06 <SecOutput+0xac>
  a30bcc:	2a0793bb          	bnei	a5,42,a30bda <SecOutput+0x180>
  a30bd0:	4018                	lw	a4,0(s0)
  a30bd2:	0411                	addi	s0,s0,4
  a30bd4:	da3a                	sw	a4,52(sp)
  a30bd6:	4705                	li	a4,1
  a30bd8:	de3a                	sw	a4,60(sp)
  a30bda:	56f2                	lw	a3,60(sp)
  a30bdc:	5752                	lw	a4,52(sp)
  a30bde:	e285                	bnez	a3,a30bfe <SecOutput+0x1a4>
  a30be0:	0147 ae14 069f      	l.li	a3,0x147ae14
  a30be6:	fae6c4e3          	blt	a3,a4,a30b8e <SecOutput+0x134>
  a30bea:	00271693          	slli	a3,a4,0x2
  a30bee:	fd078793          	addi	a5,a5,-48
  a30bf2:	9736                	add	a4,a4,a3
  a30bf4:	9f81                	uxtb	a5
  a30bf6:	02e7879b          	addshf	a5,a5,a4,sll,1
  a30bfa:	da3e                	sw	a5,52(sp)
  a30bfc:	b729                	j	a30b06 <SecOutput+0xac>
  a30bfe:	00075463          	bgez	a4,a30c06 <SecOutput+0x1ac>
  a30c02:	57fd                	li	a5,-1
  a30c04:	da3e                	sw	a5,52(sp)
  a30c06:	5752                	lw	a4,52(sp)
  a30c08:	bf55                	j	a30bbc <SecOutput+0x162>
  a30c0a:	06a00713          	li	a4,106
  a30c0e:	08e78763          	beq	a5,a4,a30c9c <SecOutput+0x242>
  a30c12:	04f76063          	bltu	a4,a5,a30c52 <SecOutput+0x1f8>
  a30c16:	04c00713          	li	a4,76
  a30c1a:	04e78663          	beq	a5,a4,a30c66 <SecOutput+0x20c>
  a30c1e:	00f76e63          	bltu	a4,a5,a30c3a <SecOutput+0x1e0>
  a30c22:	49b7993b          	bnei	a5,73,a30b06 <SecOutput+0xac>
  a30c26:	319c                	lbu	a5,1(a1)
  a30c28:	36179fbb          	bnei	a5,54,a30ca6 <SecOutput+0x24c>
  a30c2c:	21bc                	lbu	a5,2(a1)
  a30c2e:	34279d3b          	bnei	a5,52,a30ce2 <SecOutput+0x288>
  a30c32:	00358a13          	addi	s4,a1,3
  a30c36:	6721                	lui	a4,0x8
  a30c38:	a091                	j	a30c7c <SecOutput+0x222>
  a30c3a:	5a078fbb          	beqi	a5,90,a30c78 <SecOutput+0x21e>
  a30c3e:	68b7923b          	bnei	a5,104,a30b06 <SecOutput+0xac>
  a30c42:	3194                	lbu	a3,1(a1)
  a30c44:	5732                	lw	a4,44(sp)
  a30c46:	0af69763          	bne	a3,a5,a30cf4 <SecOutput+0x29a>
  a30c4a:	00258a13          	addi	s4,a1,2
  a30c4e:	6789                	lui	a5,0x2
  a30c50:	a081                	j	a30c90 <SecOutput+0x236>
  a30c52:	07400713          	li	a4,116
  a30c56:	04e78663          	beq	a5,a4,a30ca2 <SecOutput+0x248>
  a30c5a:	00f76b63          	bltu	a4,a5,a30c70 <SecOutput+0x216>
  a30c5e:	6c07893b          	beqi	a5,108,a30c82 <SecOutput+0x228>
  a30c62:	71a7993b          	bnei	a5,113,a30b06 <SecOutput+0xac>
  a30c66:	57b2                	lw	a5,44(sp)
  a30c68:	0000 1400 071f      	l.li	a4,0x1400
  a30c6e:	a801                	j	a30c7e <SecOutput+0x224>
  a30c70:	772782bb          	beqi	a5,119,a30cfa <SecOutput+0x2a0>
  a30c74:	7aa794bb          	bnei	a5,122,a30b06 <SecOutput+0xac>
  a30c78:	00020737          	lui	a4,0x20
  a30c7c:	57b2                	lw	a5,44(sp)
  a30c7e:	8fd9                	or	a5,a5,a4
  a30c80:	bdd9                	j	a30b56 <SecOutput+0xfc>
  a30c82:	3194                	lbu	a3,1(a1)
  a30c84:	5732                	lw	a4,44(sp)
  a30c86:	00f69763          	bne	a3,a5,a30c94 <SecOutput+0x23a>
  a30c8a:	00258a13          	addi	s4,a1,2
  a30c8e:	6785                	lui	a5,0x1
  a30c90:	8f5d                	or	a4,a4,a5
  a30c92:	a019                	j	a30c98 <SecOutput+0x23e>
  a30c94:	01076713          	ori	a4,a4,16
  a30c98:	d63a                	sw	a4,44(sp)
  a30c9a:	b5b5                	j	a30b06 <SecOutput+0xac>
  a30c9c:	00040737          	lui	a4,0x40
  a30ca0:	bff1                	j	a30c7c <SecOutput+0x222>
  a30ca2:	6741                	lui	a4,0x10
  a30ca4:	bfe1                	j	a30c7c <SecOutput+0x222>
  a30ca6:	3307963b          	bnei	a5,51,a30cbe <SecOutput+0x264>
  a30caa:	21bc                	lbu	a5,2(a1)
  a30cac:	32079dbb          	bnei	a5,50,a30ce2 <SecOutput+0x288>
  a30cb0:	57b2                	lw	a5,44(sp)
  a30cb2:	7761                	lui	a4,0xffff8
  a30cb4:	177d                	addi	a4,a4,-1 # ffff7fff <_gp_+0xff5c3843>
  a30cb6:	00358a13          	addi	s4,a1,3
  a30cba:	8ff9                	and	a5,a5,a4
  a30cbc:	bd69                	j	a30b56 <SecOutput+0xfc>
  a30cbe:	f9c78713          	addi	a4,a5,-100 # f9c <__bss_size__+0x998>
  a30cc2:	0ff77693          	andi	a3,a4,255
  a30cc6:	1206f53b          	bgeui	a3,18,a30cda <SecOutput+0x280>
  a30cca:	0002 0821 071f      	l.li	a4,0x20821
  a30cd0:	00d75733          	srl	a4,a4,a3
  a30cd4:	8b05                	andi	a4,a4,1
  a30cd6:	e20718e3          	bnez	a4,a30b06 <SecOutput+0xac>
  a30cda:	0df7f793          	andi	a5,a5,223
  a30cde:	58878a3b          	beqi	a5,88,a30b06 <SecOutput+0xac>
  a30ce2:	000ca783          	lw	a5,0(s9)
  a30ce6:	17fd                	addi	a5,a5,-1
  a30ce8:	00fca023          	sw	a5,0(s9)
  a30cec:	0007dc63          	bgez	a5,a30d04 <SecOutput+0x2aa>
  a30cf0:	57fd                	li	a5,-1
  a30cf2:	a02d                	j	a30d1c <SecOutput+0x2c2>
  a30cf4:	02076713          	ori	a4,a4,32
  a30cf8:	b745                	j	a30c98 <SecOutput+0x23e>
  a30cfa:	57b2                	lw	a5,44(sp)
  a30cfc:	0000 0800 071f      	l.li	a4,0x800
  a30d02:	bfb5                	j	a30c7e <SecOutput+0x224>
  a30d04:	004ca783          	lw	a5,4(s9)
  a30d08:	04900713          	li	a4,73
  a30d0c:	a398                	sb	a4,0(a5)
  a30d0e:	004ca783          	lw	a5,4(s9)
  a30d12:	0785                	addi	a5,a5,1
  a30d14:	00fca223          	sw	a5,4(s9)
  a30d18:	47d2                	lw	a5,20(sp)
  a30d1a:	0785                	addi	a5,a5,1
  a30d1c:	ca3e                	sw	a5,20(sp)
  a30d1e:	4981                	li	s3,0
  a30d20:	b3dd                	j	a30b06 <SecOutput+0xac>
  a30d22:	6847f23b          	bgeui	a5,104,a30e2a <SecOutput+0x3d0>
  a30d26:	06500713          	li	a4,101
  a30d2a:	e6e7f2e3          	bgeu	a5,a4,a30b8e <SecOutput+0x134>
  a30d2e:	05300713          	li	a4,83
  a30d32:	1ae78963          	beq	a5,a4,a30ee4 <SecOutput+0x48a>
  a30d36:	08f76f63          	bltu	a4,a5,a30dd4 <SecOutput+0x37a>
  a30d3a:	04300713          	li	a4,67
  a30d3e:	14e78f63          	beq	a5,a4,a30e9c <SecOutput+0x442>
  a30d42:	00e7e663          	bltu	a5,a4,a30d4e <SecOutput+0x2f4>
  a30d46:	fbb78793          	addi	a5,a5,-69
  a30d4a:	0397e13b          	bltui	a5,3,a30b8e <SecOutput+0x134>
  a30d4e:	5782                	lw	a5,32(sp)
  a30d50:	5642                	lw	a2,48(sp)
  a30d52:	8e1d                	sub	a2,a2,a5
  a30d54:	4796                	lw	a5,68(sp)
  a30d56:	8e1d                	sub	a2,a2,a5
  a30d58:	57b2                	lw	a5,44(sp)
  a30d5a:	c0b2                	sw	a2,64(sp)
  a30d5c:	8bb1                	andi	a5,a5,12
  a30d5e:	eb89                	bnez	a5,a30d70 <SecOutput+0x316>
  a30d60:	00c05863          	blez	a2,a30d70 <SecOutput+0x316>
  a30d64:	0854                	addi	a3,sp,20
  a30d66:	02000593          	li	a1,32
  a30d6a:	8566                	mv	a0,s9
  a30d6c:	cc3ff0ef          	jal	ra,a30a2e <SecWriteMultiChar>
  a30d70:	4696                	lw	a3,68(sp)
  a30d72:	00d05e63          	blez	a3,a30d8e <SecOutput+0x334>
  a30d76:	083c                	addi	a5,sp,24
  a30d78:	00f68633          	add	a2,a3,a5
  a30d7c:	000ca703          	lw	a4,0(s9)
  a30d80:	177d                	addi	a4,a4,-1
  a30d82:	00eca023          	sw	a4,0(s9)
  a30d86:	4a075e63          	bgez	a4,a31242 <SecOutput+0x7e8>
  a30d8a:	57fd                	li	a5,-1
  a30d8c:	ca3e                	sw	a5,20(sp)
  a30d8e:	57b2                	lw	a5,44(sp)
  a30d90:	8bb1                	andi	a5,a5,12
  a30d92:	080795bb          	bnei	a5,8,a30da8 <SecOutput+0x34e>
  a30d96:	4606                	lw	a2,64(sp)
  a30d98:	00c05863          	blez	a2,a30da8 <SecOutput+0x34e>
  a30d9c:	0854                	addi	a3,sp,20
  a30d9e:	03000593          	li	a1,48
  a30da2:	8566                	mv	a0,s9
  a30da4:	c8bff0ef          	jal	ra,a30a2e <SecWriteMultiChar>
  a30da8:	5792                	lw	a5,36(sp)
  a30daa:	4a078c63          	beqz	a5,a31262 <SecOutput+0x808>
  a30dae:	57fd                	li	a5,-1
  a30db0:	ca3e                	sw	a5,20(sp)
  a30db2:	47d2                	lw	a5,20(sp)
  a30db4:	d407c9e3          	bltz	a5,a30b06 <SecOutput+0xac>
  a30db8:	57b2                	lw	a5,44(sp)
  a30dba:	8b91                	andi	a5,a5,4
  a30dbc:	d40785e3          	beqz	a5,a30b06 <SecOutput+0xac>
  a30dc0:	4606                	lw	a2,64(sp)
  a30dc2:	d4c052e3          	blez	a2,a30b06 <SecOutput+0xac>
  a30dc6:	0854                	addi	a3,sp,20
  a30dc8:	02000593          	li	a1,32
  a30dcc:	8566                	mv	a0,s9
  a30dce:	c61ff0ef          	jal	ra,a30a2e <SecWriteMultiChar>
  a30dd2:	bb15                	j	a30b06 <SecOutput+0xac>
  a30dd4:	06300713          	li	a4,99
  a30dd8:	0ce78c63          	beq	a5,a4,a30eb0 <SecOutput+0x456>
  a30ddc:	06f76163          	bltu	a4,a5,a30e3e <SecOutput+0x3e4>
  a30de0:	58d79bbb          	bnei	a5,88,a30d4e <SecOutput+0x2f4>
  a30de4:	4741                	li	a4,16
  a30de6:	d43a                	sw	a4,40(sp)
  a30de8:	cc4a                	sw	s2,24(sp)
  a30dea:	5732                	lw	a4,44(sp)
  a30dec:	585785bb          	beqi	a5,88,a30f42 <SecOutput+0x4e8>
  a30df0:	7057953b          	bnei	a5,112,a30f44 <SecOutput+0x4ea>
  a30df4:	0000 4090 079f      	l.li	a5,0x4090
  a30dfa:	8f5d                	or	a4,a4,a5
  a30dfc:	4789                	li	a5,2
  a30dfe:	c2be                	sw	a5,68(sp)
  a30e00:	77e1                	lui	a5,0xffff8
  a30e02:	8307c793          	xori	a5,a5,-2000
  a30e06:	da02                	sw	zero,52(sp)
  a30e08:	d63a                	sw	a4,44(sp)
  a30e0a:	04f11423          	sh	a5,72(sp)
  a30e0e:	57b2                	lw	a5,44(sp)
  a30e10:	6725                	lui	a4,0x9
  a30e12:	8f7d                	and	a4,a4,a5
  a30e14:	16070a63          	beqz	a4,a30f88 <SecOutput+0x52e>
  a30e18:	041d                	addi	s0,s0,7
  a30e1a:	ff847793          	andi	a5,s0,-8
  a30e1e:	00878413          	addi	s0,a5,8 # ffff8008 <_gp_+0xff5c384c>
  a30e22:	4384                	lw	s1,0(a5)
  a30e24:	0047ad03          	lw	s10,4(a5)
  a30e28:	a245                	j	a30fc8 <SecOutput+0x56e>
  a30e2a:	07000713          	li	a4,112
  a30e2e:	fae78be3          	beq	a5,a4,a30de4 <SecOutput+0x38a>
  a30e32:	02f76363          	bltu	a4,a5,a30e58 <SecOutput+0x3fe>
  a30e36:	6907823b          	beqi	a5,105,a30e3e <SecOutput+0x3e4>
  a30e3a:	6fc7953b          	bnei	a5,111,a30d4e <SecOutput+0x2f4>
  a30e3e:	06900693          	li	a3,105
  a30e42:	5732                	lw	a4,44(sp)
  a30e44:	00d78663          	beq	a5,a3,a30e50 <SecOutput+0x3f6>
  a30e48:	10f6ed63          	bltu	a3,a5,a30f62 <SecOutput+0x508>
  a30e4c:	64f790bb          	bnei	a5,100,a30e0e <SecOutput+0x3b4>
  a30e50:	04076713          	ori	a4,a4,64
  a30e54:	d63a                	sw	a4,44(sp)
  a30e56:	aa11                	j	a30f6a <SecOutput+0x510>
  a30e58:	75f789bb          	beqi	a5,117,a30e3e <SecOutput+0x3e4>
  a30e5c:	78e7823b          	beqi	a5,120,a30de4 <SecOutput+0x38a>
  a30e60:	73b79bbb          	bnei	a5,115,a30d4e <SecOutput+0x2f4>
  a30e64:	57b2                	lw	a5,44(sp)
  a30e66:	4014                	lw	a3,0(s0)
  a30e68:	00440493          	addi	s1,s0,4
  a30e6c:	ff77f713          	andi	a4,a5,-9
  a30e70:	d63a                	sw	a4,44(sp)
  a30e72:	0000 0810 071f      	l.li	a4,0x810
  a30e78:	8ff9                	and	a5,a5,a4
  a30e7a:	ce36                	sw	a3,28(sp)
  a30e7c:	5752                	lw	a4,52(sp)
  a30e7e:	ebd9                	bnez	a5,a30f14 <SecOutput+0x4ba>
  a30e80:	e689                	bnez	a3,a30e8a <SecOutput+0x430>
  a30e82:	00a3 3f84 079f      	l.li	a5,0xa33f84
  a30e88:	ce3e                	sw	a5,28(sp)
  a30e8a:	4572                	lw	a0,28(sp)
  a30e8c:	ff171bbb          	bnei	a4,-1,a30efa <SecOutput+0x4a0>
  a30e90:	e7df70ef          	jal	ra,a28d0c <strlen>
  a30e94:	06055b63          	bgez	a0,a30f0a <SecOutput+0x4b0>
  a30e98:	4501                	li	a0,0
  a30e9a:	a885                	j	a30f0a <SecOutput+0x4b0>
  a30e9c:	57b2                	lw	a5,44(sp)
  a30e9e:	6705                	lui	a4,0x1
  a30ea0:	83070693          	addi	a3,a4,-2000 # 830 <__bss_size__+0x22c>
  a30ea4:	8efd                	and	a3,a3,a5
  a30ea6:	e689                	bnez	a3,a30eb0 <SecOutput+0x456>
  a30ea8:	80070713          	addi	a4,a4,-2048
  a30eac:	8fd9                	or	a5,a5,a4
  a30eae:	d63e                	sw	a5,44(sp)
  a30eb0:	57b2                	lw	a5,44(sp)
  a30eb2:	4605                	li	a2,1
  a30eb4:	d032                	sw	a2,32(sp)
  a30eb6:	ff77f593          	andi	a1,a5,-9
  a30eba:	d62e                	sw	a1,44(sp)
  a30ebc:	0000 0810 059f      	l.li	a1,0x810
  a30ec2:	8fed                	and	a5,a5,a1
  a30ec4:	00440713          	addi	a4,s0,4
  a30ec8:	4014                	lw	a3,0(s0)
  a30eca:	c799                	beqz	a5,a30ed8 <SecOutput+0x47e>
  a30ecc:	00fc                	addi	a5,sp,76
  a30ece:	c6b6                	sw	a3,76(sp)
  a30ed0:	ce3e                	sw	a5,28(sp)
  a30ed2:	d232                	sw	a2,36(sp)
  a30ed4:	843a                	mv	s0,a4
  a30ed6:	bda5                	j	a30d4e <SecOutput+0x2f4>
  a30ed8:	00fc                	addi	a5,sp,76
  a30eda:	04d10623          	sb	a3,76(sp)
  a30ede:	ce3e                	sw	a5,28(sp)
  a30ee0:	d202                	sw	zero,36(sp)
  a30ee2:	bfcd                	j	a30ed4 <SecOutput+0x47a>
  a30ee4:	57b2                	lw	a5,44(sp)
  a30ee6:	6705                	lui	a4,0x1
  a30ee8:	83070693          	addi	a3,a4,-2000 # 830 <__bss_size__+0x22c>
  a30eec:	8efd                	and	a3,a3,a5
  a30eee:	fabd                	bnez	a3,a30e64 <SecOutput+0x40a>
  a30ef0:	80070713          	addi	a4,a4,-2048
  a30ef4:	8fd9                	or	a5,a5,a4
  a30ef6:	d63e                	sw	a5,44(sp)
  a30ef8:	b7b5                	j	a30e64 <SecOutput+0x40a>
  a30efa:	972a                	add	a4,a4,a0
  a30efc:	87aa                	mv	a5,a0
  a30efe:	00e78463          	beq	a5,a4,a30f06 <SecOutput+0x4ac>
  a30f02:	2394                	lbu	a3,0(a5)
  a30f04:	e691                	bnez	a3,a30f10 <SecOutput+0x4b6>
  a30f06:	40a78533          	sub	a0,a5,a0
  a30f0a:	d02a                	sw	a0,32(sp)
  a30f0c:	8426                	mv	s0,s1
  a30f0e:	b581                	j	a30d4e <SecOutput+0x2f4>
  a30f10:	0785                	addi	a5,a5,1
  a30f12:	b7f5                	j	a30efe <SecOutput+0x4a4>
  a30f14:	4785                	li	a5,1
  a30f16:	d23e                	sw	a5,36(sp)
  a30f18:	e689                	bnez	a3,a30f22 <SecOutput+0x4c8>
  a30f1a:	00a3 3f8c 079f      	l.li	a5,0xa33f8c
  a30f20:	ce3e                	sw	a5,28(sp)
  a30f22:	46f2                	lw	a3,28(sp)
  a30f24:	4781                	li	a5,0
  a30f26:	00e78463          	beq	a5,a4,a30f2e <SecOutput+0x4d4>
  a30f2a:	4290                	lw	a2,0(a3)
  a30f2c:	ea01                	bnez	a2,a30f3c <SecOutput+0x4e2>
  a30f2e:	20000737          	lui	a4,0x20000
  a30f32:	00e7e363          	bltu	a5,a4,a30f38 <SecOutput+0x4de>
  a30f36:	4781                	li	a5,0
  a30f38:	d03e                	sw	a5,32(sp)
  a30f3a:	bfc9                	j	a30f0c <SecOutput+0x4b2>
  a30f3c:	0785                	addi	a5,a5,1
  a30f3e:	0691                	addi	a3,a3,4
  a30f40:	b7dd                	j	a30f26 <SecOutput+0x4cc>
  a30f42:	cc56                	sw	s5,24(sp)
  a30f44:	08077713          	andi	a4,a4,128
  a30f48:	ee070be3          	beqz	a4,a30e3e <SecOutput+0x3e4>
  a30f4c:	03000713          	li	a4,48
  a30f50:	04e10423          	sb	a4,72(sp)
  a30f54:	4762                	lw	a4,24(sp)
  a30f56:	2b18                	lbu	a4,16(a4)
  a30f58:	04e104a3          	sb	a4,73(sp)
  a30f5c:	4709                	li	a4,2
  a30f5e:	c2ba                	sw	a4,68(sp)
  a30f60:	bdf9                	j	a30e3e <SecOutput+0x3e4>
  a30f62:	6f07843b          	beqi	a5,111,a30f72 <SecOutput+0x518>
  a30f66:	75a79a3b          	bnei	a5,117,a30e0e <SecOutput+0x3b4>
  a30f6a:	47a9                	li	a5,10
  a30f6c:	d43e                	sw	a5,40(sp)
  a30f6e:	cc4a                	sw	s2,24(sp)
  a30f70:	bd79                	j	a30e0e <SecOutput+0x3b4>
  a30f72:	47a1                	li	a5,8
  a30f74:	d43e                	sw	a5,40(sp)
  a30f76:	cc4a                	sw	s2,24(sp)
  a30f78:	08077793          	andi	a5,a4,128
  a30f7c:	e80789e3          	beqz	a5,a30e0e <SecOutput+0x3b4>
  a30f80:	20076713          	ori	a4,a4,512
  a30f84:	d63a                	sw	a4,44(sp)
  a30f86:	b561                	j	a30e0e <SecOutput+0x3b4>
  a30f88:	0107f693          	andi	a3,a5,16
  a30f8c:	0407f713          	andi	a4,a5,64
  a30f90:	c689                	beqz	a3,a30f9a <SecOutput+0x540>
  a30f92:	4004                	lw	s1,0(s0)
  a30f94:	0411                	addi	s0,s0,4
  a30f96:	cb4d                	beqz	a4,a31048 <SecOutput+0x5ee>
  a30f98:	a065                	j	a31040 <SecOutput+0x5e6>
  a30f9a:	01279693          	slli	a3,a5,0x12
  a30f9e:	0806d963          	bgez	a3,a31030 <SecOutput+0x5d6>
  a30fa2:	4014                	lw	a3,0(s0)
  a30fa4:	4d01                	li	s10,0
  a30fa6:	0411                	addi	s0,s0,4
  a30fa8:	0ff6f493          	andi	s1,a3,255
  a30fac:	cf11                	beqz	a4,a30fc8 <SecOutput+0x56e>
  a30fae:	06e2                	slli	a3,a3,0x18
  a30fb0:	86e1                	srai	a3,a3,0x18
  a30fb2:	0006db63          	bgez	a3,a30fc8 <SecOutput+0x56e>
  a30fb6:	fff4c493          	not	s1,s1
  a30fba:	9c81                	uxtb	s1
  a30fbc:	0485                	addi	s1,s1,1
  a30fbe:	1007e793          	ori	a5,a5,256
  a30fc2:	41f4dd13          	srai	s10,s1,0x1f
  a30fc6:	d63e                	sw	a5,44(sp)
  a30fc8:	57b2                	lw	a5,44(sp)
  a30fca:	8da6                	mv	s11,s1
  a30fcc:	876a                	mv	a4,s10
  a30fce:	0407f693          	andi	a3,a5,64
  a30fd2:	ce89                	beqz	a3,a30fec <SecOutput+0x592>
  a30fd4:	000d5c63          	bgez	s10,a30fec <SecOutput+0x592>
  a30fd8:	40900db3          	neg	s11,s1
  a30fdc:	01b036b3          	snez	a3,s11
  a30fe0:	41a00733          	neg	a4,s10
  a30fe4:	1007e793          	ori	a5,a5,256
  a30fe8:	8f15                	sub	a4,a4,a3
  a30fea:	d63e                	sw	a5,44(sp)
  a30fec:	56b2                	lw	a3,44(sp)
  a30fee:	000497b7          	lui	a5,0x49
  a30ff2:	8ff5                	and	a5,a5,a3
  a30ff4:	e391                	bnez	a5,a30ff8 <SecOutput+0x59e>
  a30ff6:	4701                	li	a4,0
  a30ff8:	24c10813          	addi	a6,sp,588
  a30ffc:	ce42                	sw	a6,28(sp)
  a30ffe:	00ede7b3          	or	a5,s11,a4
  a31002:	e3c1                	bnez	a5,a31082 <SecOutput+0x628>
  a31004:	c282                	sw	zero,68(sp)
  a31006:	d002                	sw	zero,32(sp)
  a31008:	5752                	lw	a4,52(sp)
  a3100a:	4685                	li	a3,1
  a3100c:	00074f63          	bltz	a4,a3102a <SecOutput+0x5d0>
  a31010:	57b2                	lw	a5,44(sp)
  a31012:	01179693          	slli	a3,a5,0x11
  a31016:	0006c463          	bltz	a3,a3101e <SecOutput+0x5c4>
  a3101a:	9bdd                	andi	a5,a5,-9
  a3101c:	d63e                	sw	a5,44(sp)
  a3101e:	20000793          	li	a5,512
  a31022:	00e7d363          	bge	a5,a4,a31028 <SecOutput+0x5ce>
  a31026:	da3e                	sw	a5,52(sp)
  a31028:	56d2                	lw	a3,52(sp)
  a3102a:	03000613          	li	a2,48
  a3102e:	aa69                	j	a311c8 <SecOutput+0x76e>
  a31030:	0207f693          	andi	a3,a5,32
  a31034:	ce81                	beqz	a3,a3104c <SecOutput+0x5f2>
  a31036:	4004                	lw	s1,0(s0)
  a31038:	0411                	addi	s0,s0,4
  a3103a:	c711                	beqz	a4,a31046 <SecOutput+0x5ec>
  a3103c:	04c2                	slli	s1,s1,0x10
  a3103e:	84c1                	srai	s1,s1,0x10
  a31040:	41f4dd13          	srai	s10,s1,0x1f
  a31044:	b751                	j	a30fc8 <SecOutput+0x56e>
  a31046:	9ca1                	uxth	s1
  a31048:	4d01                	li	s10,0
  a3104a:	bfbd                	j	a30fc8 <SecOutput+0x56e>
  a3104c:	00f79693          	slli	a3,a5,0xf
  a31050:	0006d763          	bgez	a3,a3105e <SecOutput+0x604>
  a31054:	4004                	lw	s1,0(s0)
  a31056:	0411                	addi	s0,s0,4
  a31058:	41f4dd13          	srai	s10,s1,0x1f
  a3105c:	b7b5                	j	a30fc8 <SecOutput+0x56e>
  a3105e:	00e79693          	slli	a3,a5,0xe
  a31062:	0006db63          	bgez	a3,a31078 <SecOutput+0x61e>
  a31066:	4004                	lw	s1,0(s0)
  a31068:	00440793          	addi	a5,s0,4
  a3106c:	41f4dd13          	srai	s10,s1,0x1f
  a31070:	e311                	bnez	a4,a31074 <SecOutput+0x61a>
  a31072:	4d01                	li	s10,0
  a31074:	843e                	mv	s0,a5
  a31076:	bf89                	j	a30fc8 <SecOutput+0x56e>
  a31078:	00d79693          	slli	a3,a5,0xd
  a3107c:	f006dbe3          	bgez	a3,a30f92 <SecOutput+0x538>
  a31080:	bb61                	j	a30e18 <SecOutput+0x3be>
  a31082:	57a2                	lw	a5,40(sp)
  a31084:	ef49                	bnez	a4,a3111e <SecOutput+0x6c4>
  a31086:	0a1791bb          	bnei	a5,10,a310cc <SecOutput+0x672>
  a3108a:	46a9                	li	a3,10
  a3108c:	4625                	li	a2,9
  a3108e:	4772                	lw	a4,28(sp)
  a31090:	fff70793          	addi	a5,a4,-1 # 1fffffff <_gp_+0x1f5cb843>
  a31094:	ce3e                	sw	a5,28(sp)
  a31096:	02ddf7b3          	remu	a5,s11,a3
  a3109a:	97ca                	add	a5,a5,s2
  a3109c:	239c                	lbu	a5,0(a5)
  a3109e:	fef70fa3          	sb	a5,-1(a4)
  a310a2:	002dd793          	srli	a5,s11,0x2
  a310a6:	43b7879b          	addshf	a5,a5,s11,srl,1
  a310aa:	48f7879b          	addshf	a5,a5,a5,srl,4
  a310ae:	50f7879b          	addshf	a5,a5,a5,srl,8
  a310b2:	60f7879b          	addshf	a5,a5,a5,srl,16
  a310b6:	838d                	srli	a5,a5,0x3
  a310b8:	00279513          	slli	a0,a5,0x2
  a310bc:	953e                	add	a0,a0,a5
  a310be:	02ad951b          	subshf	a0,s11,a0,sll,1
  a310c2:	04a67c63          	bgeu	a2,a0,a3111a <SecOutput+0x6c0>
  a310c6:	0785                	addi	a5,a5,1 # 49001 <__heap_size+0x2ba09>
  a310c8:	8dbe                	mv	s11,a5
  a310ca:	b7d1                	j	a3108e <SecOutput+0x634>
  a310cc:	100789bb          	beqi	a5,16,a310f2 <SecOutput+0x698>
  a310d0:	0817903b          	bnei	a5,8,a31110 <SecOutput+0x6b6>
  a310d4:	4772                	lw	a4,28(sp)
  a310d6:	fff70793          	addi	a5,a4,-1
  a310da:	ce3e                	sw	a5,28(sp)
  a310dc:	007df793          	andi	a5,s11,7
  a310e0:	97ca                	add	a5,a5,s2
  a310e2:	239c                	lbu	a5,0(a5)
  a310e4:	003ddd93          	srli	s11,s11,0x3
  a310e8:	fef70fa3          	sb	a5,-1(a4)
  a310ec:	fe0d94e3          	bnez	s11,a310d4 <SecOutput+0x67a>
  a310f0:	a005                	j	a31110 <SecOutput+0x6b6>
  a310f2:	4772                	lw	a4,28(sp)
  a310f4:	00fdf693          	andi	a3,s11,15
  a310f8:	004ddd93          	srli	s11,s11,0x4
  a310fc:	fff70793          	addi	a5,a4,-1
  a31100:	ce3e                	sw	a5,28(sp)
  a31102:	47e2                	lw	a5,24(sp)
  a31104:	97b6                	add	a5,a5,a3
  a31106:	239c                	lbu	a5,0(a5)
  a31108:	fef70fa3          	sb	a5,-1(a4)
  a3110c:	fe0d93e3          	bnez	s11,a310f2 <SecOutput+0x698>
  a31110:	47f2                	lw	a5,28(sp)
  a31112:	40f80833          	sub	a6,a6,a5
  a31116:	d042                	sw	a6,32(sp)
  a31118:	bdc5                	j	a31008 <SecOutput+0x5ae>
  a3111a:	f7dd                	bnez	a5,a310c8 <SecOutput+0x66e>
  a3111c:	bfd5                	j	a31110 <SecOutput+0x6b6>
  a3111e:	0a078cbb          	beqi	a5,10,a31150 <SecOutput+0x6f6>
  a31122:	10178b3b          	beqi	a5,16,a3118e <SecOutput+0x734>
  a31126:	08f79abb          	bnei	a5,8,a31110 <SecOutput+0x6b6>
  a3112a:	46f2                	lw	a3,28(sp)
  a3112c:	fff68793          	addi	a5,a3,-1
  a31130:	ce3e                	sw	a5,28(sp)
  a31132:	007df793          	andi	a5,s11,7
  a31136:	97ca                	add	a5,a5,s2
  a31138:	239c                	lbu	a5,0(a5)
  a3113a:	003ddd93          	srli	s11,s11,0x3
  a3113e:	3aedad9b          	orshf	s11,s11,a4,sll,29
  a31142:	830d                	srli	a4,a4,0x3
  a31144:	fef68fa3          	sb	a5,-1(a3)
  a31148:	00ede7b3          	or	a5,s11,a4
  a3114c:	fff9                	bnez	a5,a3112a <SecOutput+0x6d0>
  a3114e:	b7c9                	j	a31110 <SecOutput+0x6b6>
  a31150:	47f2                	lw	a5,28(sp)
  a31152:	4629                	li	a2,10
  a31154:	856e                	mv	a0,s11
  a31156:	fff78693          	addi	a3,a5,-1
  a3115a:	ce36                	sw	a3,28(sp)
  a3115c:	85ba                	mv	a1,a4
  a3115e:	4681                	li	a3,0
  a31160:	c642                	sw	a6,12(sp)
  a31162:	c43e                	sw	a5,8(sp)
  a31164:	c23a                	sw	a4,4(sp)
  a31166:	c3af70ef          	jal	ra,a285a0 <__umoddi3>
  a3116a:	954a                	add	a0,a0,s2
  a3116c:	2114                	lbu	a3,0(a0)
  a3116e:	4712                	lw	a4,4(sp)
  a31170:	47a2                	lw	a5,8(sp)
  a31172:	856e                	mv	a0,s11
  a31174:	85ba                	mv	a1,a4
  a31176:	fed78fa3          	sb	a3,-1(a5)
  a3117a:	4629                	li	a2,10
  a3117c:	4681                	li	a3,0
  a3117e:	c1ef70ef          	jal	ra,a2859c <__udivdi3>
  a31182:	872e                	mv	a4,a1
  a31184:	8dc9                	or	a1,a1,a0
  a31186:	8daa                	mv	s11,a0
  a31188:	4832                	lw	a6,12(sp)
  a3118a:	f1f9                	bnez	a1,a31150 <SecOutput+0x6f6>
  a3118c:	b751                	j	a31110 <SecOutput+0x6b6>
  a3118e:	46f2                	lw	a3,28(sp)
  a31190:	00fdf613          	andi	a2,s11,15
  a31194:	004ddd93          	srli	s11,s11,0x4
  a31198:	fff68793          	addi	a5,a3,-1
  a3119c:	ce3e                	sw	a5,28(sp)
  a3119e:	47e2                	lw	a5,24(sp)
  a311a0:	38edad9b          	orshf	s11,s11,a4,sll,28
  a311a4:	8311                	srli	a4,a4,0x4
  a311a6:	97b2                	add	a5,a5,a2
  a311a8:	239c                	lbu	a5,0(a5)
  a311aa:	fef68fa3          	sb	a5,-1(a3)
  a311ae:	00ede7b3          	or	a5,s11,a4
  a311b2:	fff1                	bnez	a5,a3118e <SecOutput+0x734>
  a311b4:	bfb1                	j	a31110 <SecOutput+0x6b6>
  a311b6:	47f2                	lw	a5,28(sp)
  a311b8:	fff78713          	addi	a4,a5,-1
  a311bc:	ce3a                	sw	a4,28(sp)
  a311be:	fec78fa3          	sb	a2,-1(a5)
  a311c2:	5782                	lw	a5,32(sp)
  a311c4:	0785                	addi	a5,a5,1
  a311c6:	d03e                	sw	a5,32(sp)
  a311c8:	5702                	lw	a4,32(sp)
  a311ca:	fed746e3          	blt	a4,a3,a311b6 <SecOutput+0x75c>
  a311ce:	57b2                	lw	a5,44(sp)
  a311d0:	2007f793          	andi	a5,a5,512
  a311d4:	c385                	beqz	a5,a311f4 <SecOutput+0x79a>
  a311d6:	47f2                	lw	a5,28(sp)
  a311d8:	c701                	beqz	a4,a311e0 <SecOutput+0x786>
  a311da:	2398                	lbu	a4,0(a5)
  a311dc:	3007063b          	beqi	a4,48,a311f4 <SecOutput+0x79a>
  a311e0:	fff78713          	addi	a4,a5,-1
  a311e4:	ce3a                	sw	a4,28(sp)
  a311e6:	03000713          	li	a4,48
  a311ea:	fee78fa3          	sb	a4,-1(a5)
  a311ee:	5782                	lw	a5,32(sp)
  a311f0:	0785                	addi	a5,a5,1
  a311f2:	d03e                	sw	a5,32(sp)
  a311f4:	57b2                	lw	a5,44(sp)
  a311f6:	0407f713          	andi	a4,a5,64
  a311fa:	cb11                	beqz	a4,a3120e <SecOutput+0x7b4>
  a311fc:	1007f713          	andi	a4,a5,256
  a31200:	c70d                	beqz	a4,a3122a <SecOutput+0x7d0>
  a31202:	02d00713          	li	a4,45
  a31206:	04e10423          	sb	a4,72(sp)
  a3120a:	4705                	li	a4,1
  a3120c:	c2ba                	sw	a4,68(sp)
  a3120e:	01a4e4b3          	or	s1,s1,s10
  a31212:	b2049ee3          	bnez	s1,a30d4e <SecOutput+0x2f4>
  a31216:	01179713          	slli	a4,a5,0x11
  a3121a:	b2075ae3          	bgez	a4,a30d4e <SecOutput+0x2f4>
  a3121e:	4715                	li	a4,5
  a31220:	9bdd                	andi	a5,a5,-9
  a31222:	ce62                	sw	s8,28(sp)
  a31224:	d03a                	sw	a4,32(sp)
  a31226:	d63e                	sw	a5,44(sp)
  a31228:	b61d                	j	a30d4e <SecOutput+0x2f4>
  a3122a:	0017f713          	andi	a4,a5,1
  a3122e:	c701                	beqz	a4,a31236 <SecOutput+0x7dc>
  a31230:	02b00713          	li	a4,43
  a31234:	bfc9                	j	a31206 <SecOutput+0x7ac>
  a31236:	0027f713          	andi	a4,a5,2
  a3123a:	db71                	beqz	a4,a3120e <SecOutput+0x7b4>
  a3123c:	02000713          	li	a4,32
  a31240:	b7d9                	j	a31206 <SecOutput+0x7ac>
  a31242:	004ca703          	lw	a4,4(s9)
  a31246:	0307c583          	lbu	a1,48(a5)
  a3124a:	0785                	addi	a5,a5,1
  a3124c:	a30c                	sb	a1,0(a4)
  a3124e:	004ca703          	lw	a4,4(s9)
  a31252:	0705                	addi	a4,a4,1
  a31254:	00eca223          	sw	a4,4(s9)
  a31258:	b2f612e3          	bne	a2,a5,a30d7c <SecOutput+0x322>
  a3125c:	47d2                	lw	a5,20(sp)
  a3125e:	97b6                	add	a5,a5,a3
  a31260:	b635                	j	a30d8c <SecOutput+0x332>
  a31262:	45f2                	lw	a1,28(sp)
  a31264:	5482                	lw	s1,32(sp)
  a31266:	000ca703          	lw	a4,0(s9)
  a3126a:	87ae                	mv	a5,a1
  a3126c:	009586b3          	add	a3,a1,s1
  a31270:	06974963          	blt	a4,s1,a312e2 <SecOutput+0x888>
  a31274:	0c04ddbb          	bgei	s1,12,a312aa <SecOutput+0x850>
  a31278:	87ae                	mv	a5,a1
  a3127a:	a819                	j	a31290 <SecOutput+0x836>
  a3127c:	004ca703          	lw	a4,4(s9)
  a31280:	2394                	lbu	a3,0(a5)
  a31282:	0785                	addi	a5,a5,1
  a31284:	a314                	sb	a3,0(a4)
  a31286:	004ca703          	lw	a4,4(s9)
  a3128a:	0705                	addi	a4,a4,1
  a3128c:	00eca223          	sw	a4,4(s9)
  a31290:	40b78733          	sub	a4,a5,a1
  a31294:	fe9744e3          	blt	a4,s1,a3127c <SecOutput+0x822>
  a31298:	000ca783          	lw	a5,0(s9)
  a3129c:	8f85                	sub	a5,a5,s1
  a3129e:	00fca023          	sw	a5,0(s9)
  a312a2:	47d2                	lw	a5,20(sp)
  a312a4:	94be                	add	s1,s1,a5
  a312a6:	ca26                	sw	s1,20(sp)
  a312a8:	b629                	j	a30db2 <SecOutput+0x358>
  a312aa:	004ca503          	lw	a0,4(s9)
  a312ae:	8626                	mv	a2,s1
  a312b0:	fa2f70ef          	jal	ra,a28a52 <memcpy>
  a312b4:	004ca783          	lw	a5,4(s9)
  a312b8:	97a6                	add	a5,a5,s1
  a312ba:	00fca223          	sw	a5,4(s9)
  a312be:	bfe9                	j	a31298 <SecOutput+0x83e>
  a312c0:	000ca703          	lw	a4,0(s9)
  a312c4:	177d                	addi	a4,a4,-1
  a312c6:	00eca023          	sw	a4,0(s9)
  a312ca:	ae0742e3          	bltz	a4,a30dae <SecOutput+0x354>
  a312ce:	004ca703          	lw	a4,4(s9)
  a312d2:	2390                	lbu	a2,0(a5)
  a312d4:	0785                	addi	a5,a5,1
  a312d6:	a310                	sb	a2,0(a4)
  a312d8:	004ca703          	lw	a4,4(s9)
  a312dc:	0705                	addi	a4,a4,1
  a312de:	00eca223          	sw	a4,4(s9)
  a312e2:	40f68733          	sub	a4,a3,a5
  a312e6:	fce04de3          	bgtz	a4,a312c0 <SecOutput+0x866>
  a312ea:	bf65                	j	a312a2 <SecOutput+0x848>

00a312ec <SecVsnprintfImpl>:
  a312ec:	8138                	push	{ra,s0-s1},-32
  a312ee:	84ae                	mv	s1,a1
  a312f0:	842a                	mv	s0,a0
  a312f2:	85b2                	mv	a1,a2
  a312f4:	c62a                	sw	a0,12(sp)
  a312f6:	8636                	mv	a2,a3
  a312f8:	0028                	addi	a0,sp,8
  a312fa:	c426                	sw	s1,8(sp)
  a312fc:	f5eff0ef          	jal	ra,a30a5a <SecOutput>
  a31300:	00054b63          	bltz	a0,a31316 <SecVsnprintfImpl+0x2a>
  a31304:	47a2                	lw	a5,8(sp)
  a31306:	17fd                	addi	a5,a5,-1
  a31308:	c43e                	sw	a5,8(sp)
  a3130a:	0007c663          	bltz	a5,a31316 <SecVsnprintfImpl+0x2a>
  a3130e:	47b2                	lw	a5,12(sp)
  a31310:	00078023          	sb	zero,0(a5)
  a31314:	8134                	popret	{ra,s0-s1},32
  a31316:	47a2                	lw	a5,8(sp)
  a31318:	0007d763          	bgez	a5,a31326 <SecVsnprintfImpl+0x3a>
  a3131c:	9426                	add	s0,s0,s1
  a3131e:	fe040fa3          	sb	zero,-1(s0)
  a31322:	5579                	li	a0,-2
  a31324:	bfc5                	j	a31314 <SecVsnprintfImpl+0x28>
  a31326:	00040023          	sb	zero,0(s0)
  a3132a:	557d                	li	a0,-1
  a3132c:	b7e5                	j	a31314 <SecVsnprintfImpl+0x28>

00a3132e <vsnprintf_s>:
  a3132e:	8028                	push	{ra,s0},-16
  a31330:	8832                	mv	a6,a2
  a31332:	842a                	mv	s0,a0
  a31334:	87ae                	mv	a5,a1
  a31336:	8636                	mv	a2,a3
  a31338:	ca8d                	beqz	a3,a3136a <vsnprintf_s+0x3c>
  a3133a:	e119                	bnez	a0,a31340 <vsnprintf_s+0x12>
  a3133c:	557d                	li	a0,-1
  a3133e:	8024                	popret	{ra,s0},16
  a31340:	ddf5                	beqz	a1,a3133c <vsnprintf_s+0xe>
  a31342:	fe05cde3          	bltz	a1,a3133c <vsnprintf_s+0xe>
  a31346:	800008b7          	lui	a7,0x80000
  a3134a:	ffe8c893          	xori	a7,a7,-2
  a3134e:	0308e063          	bltu	a7,a6,a3136e <vsnprintf_s+0x40>
  a31352:	86ba                	mv	a3,a4
  a31354:	02b87263          	bgeu	a6,a1,a31378 <vsnprintf_s+0x4a>
  a31358:	00180593          	addi	a1,a6,1
  a3135c:	f91ff0ef          	jal	ra,a312ec <SecVsnprintfImpl>
  a31360:	fef5073b          	beqi	a0,-2,a3133c <vsnprintf_s+0xe>
  a31364:	fc055de3          	bgez	a0,a3133e <vsnprintf_s+0x10>
  a31368:	a029                	j	a31372 <vsnprintf_s+0x44>
  a3136a:	d969                	beqz	a0,a3133c <vsnprintf_s+0xe>
  a3136c:	d9e1                	beqz	a1,a3133c <vsnprintf_s+0xe>
  a3136e:	fc07c7e3          	bltz	a5,a3133c <vsnprintf_s+0xe>
  a31372:	00040023          	sb	zero,0(s0)
  a31376:	b7d9                	j	a3133c <vsnprintf_s+0xe>
  a31378:	f75ff0ef          	jal	ra,a312ec <SecVsnprintfImpl>
  a3137c:	b7e5                	j	a31364 <vsnprintf_s+0x36>
  a3137e:	0000                	unimp
  a31380:	04f9                	addi	s1,s1,30
  a31382:	0000                	unimp
  a31384:	00000003          	lb	zero,0(zero) # 0 <__data_size__-0x1e8>
	...

00a31390 <excp_vect_table>:
  a31390:	d0a4                	sw	s1,96(s1)
  a31392:	00a2                	slli	ra,ra,0x8
  a31394:	d0ba                	sw	a4,96(sp)
  a31396:	00a2                	slli	ra,ra,0x8
  a31398:	d0d0                	sw	a2,36(s1)
  a3139a:	00a2                	slli	ra,ra,0x8
  a3139c:	d180                	sw	s0,32(a1)
  a3139e:	00a2                	slli	ra,ra,0x8
  a313a0:	d0e6                	sw	s9,96(sp)
  a313a2:	00a2                	slli	ra,ra,0x8
  a313a4:	d0fc                	sw	a5,100(s1)
  a313a6:	00a2                	slli	ra,ra,0x8
  a313a8:	d112                	sw	tp,160(sp)
  a313aa:	00a2                	slli	ra,ra,0x8
  a313ac:	d128                	sw	a0,96(a0)
  a313ae:	00a2                	slli	ra,ra,0x8
  a313b0:	d13e                	sw	a5,160(sp)
  a313b2:	00a2                	slli	ra,ra,0x8
  a313b4:	d154                	sw	a3,36(a0)
  a313b6:	00a2                	slli	ra,ra,0x8
  a313b8:	d08e                	sw	gp,96(sp)
  a313ba:	00a2                	slli	ra,ra,0x8
  a313bc:	d16a                	sw	s10,160(sp)
  a313be:	00a2                	slli	ra,ra,0x8
  a313c0:	d196                	sw	t0,224(sp)
  a313c2:	00a2                	slli	ra,ra,0x8
  a313c4:	d1ac                	sw	a1,96(a1)
  a313c6:	00a2                	slli	ra,ra,0x8
  a313c8:	d08e                	sw	gp,96(sp)
  a313ca:	00a2                	slli	ra,ra,0x8
  a313cc:	d1c2                	sw	a6,224(sp)
  a313ce:	00a2                	slli	ra,ra,0x8
  a313d0:	d1d8                	sw	a4,36(a1)
  a313d2:	00a2                	slli	ra,ra,0x8
  a313d4:	d1ee                	sw	s11,224(sp)
  a313d6:	00a2                	slli	ra,ra,0x8

00a313d8 <excp_vect_table_end>:
	...

00a313e0 <trap_entry>:
  a313e0:	7175                	addi	sp,sp,-144
  a313e2:	d06e                	sw	s11,32(sp)
  a313e4:	d26a                	sw	s10,36(sp)
  a313e6:	d466                	sw	s9,40(sp)
  a313e8:	d662                	sw	s8,44(sp)
  a313ea:	d85e                	sw	s7,48(sp)
  a313ec:	da5a                	sw	s6,52(sp)
  a313ee:	dc56                	sw	s5,56(sp)
  a313f0:	de52                	sw	s4,60(sp)
  a313f2:	c0ce                	sw	s3,64(sp)
  a313f4:	c2ca                	sw	s2,68(sp)
  a313f6:	c4a6                	sw	s1,72(sp)
  a313f8:	c6a2                	sw	s0,76(sp)
  a313fa:	c8fe                	sw	t6,80(sp)
  a313fc:	cafa                	sw	t5,84(sp)
  a313fe:	ccf6                	sw	t4,88(sp)
  a31400:	cef2                	sw	t3,92(sp)
  a31402:	d0c6                	sw	a7,96(sp)
  a31404:	d2c2                	sw	a6,100(sp)
  a31406:	d4be                	sw	a5,104(sp)
  a31408:	d6ba                	sw	a4,108(sp)
  a3140a:	d8b6                	sw	a3,112(sp)
  a3140c:	dab2                	sw	a2,116(sp)
  a3140e:	dcae                	sw	a1,120(sp)
  a31410:	deaa                	sw	a0,124(sp)
  a31412:	c11e                	sw	t2,128(sp)
  a31414:	c31a                	sw	t1,132(sp)
  a31416:	c516                	sw	t0,136(sp)
  a31418:	c706                	sw	ra,140(sp)
  a3141a:	fc202a73          	csrr	s4,0xfc2
  a3141e:	c052                	sw	s4,0(sp)
  a31420:	342029f3          	csrr	s3,mcause
  a31424:	c24e                	sw	s3,4(sp)
  a31426:	34302973          	csrr	s2,mtval
  a3142a:	c44a                	sw	s2,8(sp)
  a3142c:	c60e                	sw	gp,12(sp)
  a3142e:	300024f3          	csrr	s1,mstatus
  a31432:	c826                	sw	s1,16(sp)
  a31434:	34102473          	csrr	s0,mepc
  a31438:	ca22                	sw	s0,20(sp)
  a3143a:	cc12                	sw	tp,24(sp)
  a3143c:	ce0a                	sw	sp,28(sp)
  a3143e:	30047073          	csrci	mstatus,8
  a31442:	34011073          	csrw	mscratch,sp
  a31446:	fffd4117          	auipc	sp,0xfffd4
  a3144a:	0ba10113          	addi	sp,sp,186 # a05500 <g_flash_cmd_funcs>
  a3144e:	42a1                	li	t0,8
  a31450:	00599463          	bne	s3,t0,a31458 <trap_entry+0x78>
  a31454:	8fef706f          	j	a28552 <handle_syscall>
  a31458:	00299293          	slli	t0,s3,0x2
  a3145c:	00000317          	auipc	t1,0x0
  a31460:	f3430313          	addi	t1,t1,-204 # a31390 <excp_vect_table>
  a31464:	00000397          	auipc	t2,0x0
  a31468:	f7438393          	addi	t2,t2,-140 # a313d8 <excp_vect_table_end>
  a3146c:	34002573          	csrr	a0,mscratch
  a31470:	929a                	add	t0,t0,t1
  a31472:	0472ff63          	bgeu	t0,t2,a314d0 <trap_entry+0xf0>
  a31476:	0002a283          	lw	t0,0(t0) # 1000000 <_gp_+0x5cb844>
  a3147a:	971fb0ef          	jal	ra,a2cdea <do_hard_fault_handler>
  a3147e:	34002173          	csrr	sp,mscratch
  a31482:	6289                	lui	t0,0x2
  a31484:	80028293          	addi	t0,t0,-2048 # 1800 <ccause+0x83e>
  a31488:	3002a073          	csrs	mstatus,t0
  a3148c:	42d2                	lw	t0,20(sp)
  a3148e:	34129073          	csrw	mepc,t0
  a31492:	5d82                	lw	s11,32(sp)
  a31494:	5d12                	lw	s10,36(sp)
  a31496:	5ca2                	lw	s9,40(sp)
  a31498:	5c32                	lw	s8,44(sp)
  a3149a:	5bc2                	lw	s7,48(sp)
  a3149c:	5b52                	lw	s6,52(sp)
  a3149e:	5ae2                	lw	s5,56(sp)
  a314a0:	5a72                	lw	s4,60(sp)
  a314a2:	4986                	lw	s3,64(sp)
  a314a4:	4916                	lw	s2,68(sp)
  a314a6:	44a6                	lw	s1,72(sp)
  a314a8:	4436                	lw	s0,76(sp)
  a314aa:	4fc6                	lw	t6,80(sp)
  a314ac:	4f56                	lw	t5,84(sp)
  a314ae:	4ee6                	lw	t4,88(sp)
  a314b0:	4e76                	lw	t3,92(sp)
  a314b2:	5886                	lw	a7,96(sp)
  a314b4:	5816                	lw	a6,100(sp)
  a314b6:	57a6                	lw	a5,104(sp)
  a314b8:	5736                	lw	a4,108(sp)
  a314ba:	56c6                	lw	a3,112(sp)
  a314bc:	5656                	lw	a2,116(sp)
  a314be:	55e6                	lw	a1,120(sp)
  a314c0:	5576                	lw	a0,124(sp)
  a314c2:	438a                	lw	t2,128(sp)
  a314c4:	431a                	lw	t1,132(sp)
  a314c6:	42aa                	lw	t0,136(sp)
  a314c8:	40ba                	lw	ra,140(sp)
  a314ca:	6149                	addi	sp,sp,144
  a314cc:	30200073          	mret
  a314d0:	bbffb06f          	j	a2d08e <do_trap_unknown>
	...

00a314dc <trap_vector>:
  a314dc:	f05ff06f          	j	a313e0 <trap_entry>
  a314e0:	d25fb06f          	j	a2d204 <default_handler>
  a314e4:	d21fb06f          	j	a2d204 <default_handler>
  a314e8:	d1dfb06f          	j	a2d204 <default_handler>
  a314ec:	d19fb06f          	j	a2d204 <default_handler>
  a314f0:	d15fb06f          	j	a2d204 <default_handler>
  a314f4:	d11fb06f          	j	a2d204 <default_handler>
  a314f8:	d0dfb06f          	j	a2d204 <default_handler>
  a314fc:	d09fb06f          	j	a2d204 <default_handler>
  a31500:	d05fb06f          	j	a2d204 <default_handler>
  a31504:	d01fb06f          	j	a2d204 <default_handler>
  a31508:	cfdfb06f          	j	a2d204 <default_handler>
  a3150c:	1700006f          	j	a3167c <nmi_vector>
  a31510:	cf5fb06f          	j	a2d204 <default_handler>
  a31514:	cf1fb06f          	j	a2d204 <default_handler>
  a31518:	cedfb06f          	j	a2d204 <default_handler>
  a3151c:	ce9fb06f          	j	a2d204 <default_handler>
  a31520:	ce5fb06f          	j	a2d204 <default_handler>
  a31524:	ce1fb06f          	j	a2d204 <default_handler>
  a31528:	cddfb06f          	j	a2d204 <default_handler>
  a3152c:	cd9fb06f          	j	a2d204 <default_handler>
  a31530:	cd5fb06f          	j	a2d204 <default_handler>
  a31534:	cd1fb06f          	j	a2d204 <default_handler>
  a31538:	ccdfb06f          	j	a2d204 <default_handler>
  a3153c:	cc9fb06f          	j	a2d204 <default_handler>
  a31540:	cc5fb06f          	j	a2d204 <default_handler>
  a31544:	2000006f          	j	a31744 <default_interrupt0_handler>
  a31548:	2a80006f          	j	a317f0 <default_interrupt1_handler>
  a3154c:	3500006f          	j	a3189c <default_interrupt2_handler>
  a31550:	3f80006f          	j	a31948 <default_interrupt3_handler>
  a31554:	4a00006f          	j	a319f4 <default_interrupt4_handler>
  a31558:	5480006f          	j	a31aa0 <default_interrupt5_handler>
  a3155c:	5f00006f          	j	a31b4c <default_local_interrupt_handler>
  a31560:	5ec0006f          	j	a31b4c <default_local_interrupt_handler>
  a31564:	5e80006f          	j	a31b4c <default_local_interrupt_handler>
  a31568:	5e40006f          	j	a31b4c <default_local_interrupt_handler>
  a3156c:	5e00006f          	j	a31b4c <default_local_interrupt_handler>
  a31570:	5dc0006f          	j	a31b4c <default_local_interrupt_handler>
  a31574:	5d80006f          	j	a31b4c <default_local_interrupt_handler>
  a31578:	5d40006f          	j	a31b4c <default_local_interrupt_handler>
  a3157c:	5d00006f          	j	a31b4c <default_local_interrupt_handler>
  a31580:	5cc0006f          	j	a31b4c <default_local_interrupt_handler>
  a31584:	5c80006f          	j	a31b4c <default_local_interrupt_handler>
  a31588:	5c40006f          	j	a31b4c <default_local_interrupt_handler>
  a3158c:	5c00006f          	j	a31b4c <default_local_interrupt_handler>
  a31590:	5bc0006f          	j	a31b4c <default_local_interrupt_handler>
  a31594:	5b80006f          	j	a31b4c <default_local_interrupt_handler>
  a31598:	5b40006f          	j	a31b4c <default_local_interrupt_handler>
  a3159c:	5b00006f          	j	a31b4c <default_local_interrupt_handler>
  a315a0:	5ac0006f          	j	a31b4c <default_local_interrupt_handler>
  a315a4:	5a80006f          	j	a31b4c <default_local_interrupt_handler>
  a315a8:	5a40006f          	j	a31b4c <default_local_interrupt_handler>
  a315ac:	5a00006f          	j	a31b4c <default_local_interrupt_handler>
  a315b0:	59c0006f          	j	a31b4c <default_local_interrupt_handler>
  a315b4:	5980006f          	j	a31b4c <default_local_interrupt_handler>
  a315b8:	5940006f          	j	a31b4c <default_local_interrupt_handler>
  a315bc:	5900006f          	j	a31b4c <default_local_interrupt_handler>
  a315c0:	58c0006f          	j	a31b4c <default_local_interrupt_handler>
  a315c4:	5880006f          	j	a31b4c <default_local_interrupt_handler>
  a315c8:	5840006f          	j	a31b4c <default_local_interrupt_handler>
  a315cc:	5800006f          	j	a31b4c <default_local_interrupt_handler>
  a315d0:	57c0006f          	j	a31b4c <default_local_interrupt_handler>
  a315d4:	5780006f          	j	a31b4c <default_local_interrupt_handler>
  a315d8:	5740006f          	j	a31b4c <default_local_interrupt_handler>
  a315dc:	5700006f          	j	a31b4c <default_local_interrupt_handler>
  a315e0:	56c0006f          	j	a31b4c <default_local_interrupt_handler>
  a315e4:	5680006f          	j	a31b4c <default_local_interrupt_handler>
  a315e8:	5640006f          	j	a31b4c <default_local_interrupt_handler>
  a315ec:	5600006f          	j	a31b4c <default_local_interrupt_handler>
  a315f0:	55c0006f          	j	a31b4c <default_local_interrupt_handler>
  a315f4:	5580006f          	j	a31b4c <default_local_interrupt_handler>
  a315f8:	5540006f          	j	a31b4c <default_local_interrupt_handler>
  a315fc:	5500006f          	j	a31b4c <default_local_interrupt_handler>
  a31600:	54c0006f          	j	a31b4c <default_local_interrupt_handler>
  a31604:	5480006f          	j	a31b4c <default_local_interrupt_handler>
  a31608:	5440006f          	j	a31b4c <default_local_interrupt_handler>
  a3160c:	5400006f          	j	a31b4c <default_local_interrupt_handler>
  a31610:	53c0006f          	j	a31b4c <default_local_interrupt_handler>
  a31614:	5380006f          	j	a31b4c <default_local_interrupt_handler>
  a31618:	5340006f          	j	a31b4c <default_local_interrupt_handler>
  a3161c:	5300006f          	j	a31b4c <default_local_interrupt_handler>
  a31620:	52c0006f          	j	a31b4c <default_local_interrupt_handler>
  a31624:	5280006f          	j	a31b4c <default_local_interrupt_handler>
  a31628:	5240006f          	j	a31b4c <default_local_interrupt_handler>
  a3162c:	5200006f          	j	a31b4c <default_local_interrupt_handler>
  a31630:	51c0006f          	j	a31b4c <default_local_interrupt_handler>
  a31634:	5180006f          	j	a31b4c <default_local_interrupt_handler>
  a31638:	5140006f          	j	a31b4c <default_local_interrupt_handler>
  a3163c:	5100006f          	j	a31b4c <default_local_interrupt_handler>
  a31640:	50c0006f          	j	a31b4c <default_local_interrupt_handler>
  a31644:	5080006f          	j	a31b4c <default_local_interrupt_handler>
  a31648:	5040006f          	j	a31b4c <default_local_interrupt_handler>
  a3164c:	5000006f          	j	a31b4c <default_local_interrupt_handler>
  a31650:	4fc0006f          	j	a31b4c <default_local_interrupt_handler>
  a31654:	4f80006f          	j	a31b4c <default_local_interrupt_handler>
  a31658:	4f40006f          	j	a31b4c <default_local_interrupt_handler>
  a3165c:	4f00006f          	j	a31b4c <default_local_interrupt_handler>
  a31660:	4ec0006f          	j	a31b4c <default_local_interrupt_handler>
  a31664:	4e80006f          	j	a31b4c <default_local_interrupt_handler>
  a31668:	4e40006f          	j	a31b4c <default_local_interrupt_handler>
  a3166c:	4e00006f          	j	a31b4c <default_local_interrupt_handler>
  a31670:	4dc0006f          	j	a31b4c <default_local_interrupt_handler>
  a31674:	4d80006f          	j	a31b4c <default_local_interrupt_handler>
  a31678:	0000                	unimp
	...

00a3167c <nmi_vector>:
  a3167c:	30047073          	csrci	mstatus,8
  a31680:	7175                	addi	sp,sp,-144
  a31682:	d06e                	sw	s11,32(sp)
  a31684:	d26a                	sw	s10,36(sp)
  a31686:	d466                	sw	s9,40(sp)
  a31688:	d662                	sw	s8,44(sp)
  a3168a:	d85e                	sw	s7,48(sp)
  a3168c:	da5a                	sw	s6,52(sp)
  a3168e:	dc56                	sw	s5,56(sp)
  a31690:	de52                	sw	s4,60(sp)
  a31692:	c0ce                	sw	s3,64(sp)
  a31694:	c2ca                	sw	s2,68(sp)
  a31696:	c4a6                	sw	s1,72(sp)
  a31698:	c6a2                	sw	s0,76(sp)
  a3169a:	c8fe                	sw	t6,80(sp)
  a3169c:	cafa                	sw	t5,84(sp)
  a3169e:	ccf6                	sw	t4,88(sp)
  a316a0:	cef2                	sw	t3,92(sp)
  a316a2:	d0c6                	sw	a7,96(sp)
  a316a4:	d2c2                	sw	a6,100(sp)
  a316a6:	d4be                	sw	a5,104(sp)
  a316a8:	d6ba                	sw	a4,108(sp)
  a316aa:	d8b6                	sw	a3,112(sp)
  a316ac:	dab2                	sw	a2,116(sp)
  a316ae:	dcae                	sw	a1,120(sp)
  a316b0:	deaa                	sw	a0,124(sp)
  a316b2:	c11e                	sw	t2,128(sp)
  a316b4:	c31a                	sw	t1,132(sp)
  a316b6:	c516                	sw	t0,136(sp)
  a316b8:	c706                	sw	ra,140(sp)
  a316ba:	fc202a73          	csrr	s4,0xfc2
  a316be:	c052                	sw	s4,0(sp)
  a316c0:	342029f3          	csrr	s3,mcause
  a316c4:	c24e                	sw	s3,4(sp)
  a316c6:	34302973          	csrr	s2,mtval
  a316ca:	c44a                	sw	s2,8(sp)
  a316cc:	c60e                	sw	gp,12(sp)
  a316ce:	300024f3          	csrr	s1,mstatus
  a316d2:	c826                	sw	s1,16(sp)
  a316d4:	34102473          	csrr	s0,mepc
  a316d8:	ca22                	sw	s0,20(sp)
  a316da:	cc12                	sw	tp,24(sp)
  a316dc:	ce0a                	sw	sp,28(sp)
  a316de:	34011073          	csrw	mscratch,sp
  a316e2:	fffd4117          	auipc	sp,0xfffd4
  a316e6:	a1e10113          	addi	sp,sp,-1506 # a05100 <__irq_stack_top__>
  a316ea:	34002573          	csrr	a0,mscratch
  a316ee:	efcfb0ef          	jal	ra,a2cdea <do_hard_fault_handler>
  a316f2:	34002173          	csrr	sp,mscratch
  a316f6:	6289                	lui	t0,0x2
  a316f8:	88028293          	addi	t0,t0,-1920 # 1880 <ccause+0x8be>
  a316fc:	3002a073          	csrs	mstatus,t0
  a31700:	42d2                	lw	t0,20(sp)
  a31702:	34129073          	csrw	mepc,t0
  a31706:	5d82                	lw	s11,32(sp)
  a31708:	5d12                	lw	s10,36(sp)
  a3170a:	5ca2                	lw	s9,40(sp)
  a3170c:	5c32                	lw	s8,44(sp)
  a3170e:	5bc2                	lw	s7,48(sp)
  a31710:	5b52                	lw	s6,52(sp)
  a31712:	5ae2                	lw	s5,56(sp)
  a31714:	5a72                	lw	s4,60(sp)
  a31716:	4986                	lw	s3,64(sp)
  a31718:	4916                	lw	s2,68(sp)
  a3171a:	44a6                	lw	s1,72(sp)
  a3171c:	4436                	lw	s0,76(sp)
  a3171e:	4fc6                	lw	t6,80(sp)
  a31720:	4f56                	lw	t5,84(sp)
  a31722:	4ee6                	lw	t4,88(sp)
  a31724:	4e76                	lw	t3,92(sp)
  a31726:	5886                	lw	a7,96(sp)
  a31728:	5816                	lw	a6,100(sp)
  a3172a:	57a6                	lw	a5,104(sp)
  a3172c:	5736                	lw	a4,108(sp)
  a3172e:	56c6                	lw	a3,112(sp)
  a31730:	5656                	lw	a2,116(sp)
  a31732:	55e6                	lw	a1,120(sp)
  a31734:	5576                	lw	a0,124(sp)
  a31736:	438a                	lw	t2,128(sp)
  a31738:	431a                	lw	t1,132(sp)
  a3173a:	42aa                	lw	t0,136(sp)
  a3173c:	40ba                	lw	ra,140(sp)
  a3173e:	6149                	addi	sp,sp,144
  a31740:	30200073          	mret

00a31744 <default_interrupt0_handler>:
  a31744:	30047073          	csrci	mstatus,8
  a31748:	7119                	addi	sp,sp,-128
  a3174a:	c006                	sw	ra,0(sp)
  a3174c:	c20a                	sw	sp,4(sp)
  a3174e:	c40e                	sw	gp,8(sp)
  a31750:	c612                	sw	tp,12(sp)
  a31752:	c816                	sw	t0,16(sp)
  a31754:	ca1a                	sw	t1,20(sp)
  a31756:	cc1e                	sw	t2,24(sp)
  a31758:	ce22                	sw	s0,28(sp)
  a3175a:	d026                	sw	s1,32(sp)
  a3175c:	d22a                	sw	a0,36(sp)
  a3175e:	d42e                	sw	a1,40(sp)
  a31760:	d632                	sw	a2,44(sp)
  a31762:	d836                	sw	a3,48(sp)
  a31764:	da3a                	sw	a4,52(sp)
  a31766:	dc3e                	sw	a5,56(sp)
  a31768:	de42                	sw	a6,60(sp)
  a3176a:	c0c6                	sw	a7,64(sp)
  a3176c:	c2ca                	sw	s2,68(sp)
  a3176e:	c4ce                	sw	s3,72(sp)
  a31770:	c6d2                	sw	s4,76(sp)
  a31772:	c8d6                	sw	s5,80(sp)
  a31774:	cada                	sw	s6,84(sp)
  a31776:	ccde                	sw	s7,88(sp)
  a31778:	cee2                	sw	s8,92(sp)
  a3177a:	d0e6                	sw	s9,96(sp)
  a3177c:	d2ea                	sw	s10,100(sp)
  a3177e:	d4ee                	sw	s11,104(sp)
  a31780:	d6f2                	sw	t3,108(sp)
  a31782:	d8f6                	sw	t4,112(sp)
  a31784:	dafa                	sw	t5,116(sp)
  a31786:	dcfe                	sw	t6,120(sp)
  a31788:	341022f3          	csrr	t0,mepc
  a3178c:	de96                	sw	t0,124(sp)
  a3178e:	34011073          	csrw	mscratch,sp
  a31792:	fffd4117          	auipc	sp,0xfffd4
  a31796:	96e10113          	addi	sp,sp,-1682 # a05100 <__irq_stack_top__>
  a3179a:	c6afb0ef          	jal	ra,a2cc04 <interrupt0_handler>
  a3179e:	34002173          	csrr	sp,mscratch
  a317a2:	6289                	lui	t0,0x2
  a317a4:	88028293          	addi	t0,t0,-1920 # 1880 <ccause+0x8be>
  a317a8:	3002a073          	csrs	mstatus,t0
  a317ac:	52f6                	lw	t0,124(sp)
  a317ae:	34129073          	csrw	mepc,t0
  a317b2:	4082                	lw	ra,0(sp)
  a317b4:	42c2                	lw	t0,16(sp)
  a317b6:	4352                	lw	t1,20(sp)
  a317b8:	43e2                	lw	t2,24(sp)
  a317ba:	4472                	lw	s0,28(sp)
  a317bc:	5482                	lw	s1,32(sp)
  a317be:	5512                	lw	a0,36(sp)
  a317c0:	55a2                	lw	a1,40(sp)
  a317c2:	5632                	lw	a2,44(sp)
  a317c4:	56c2                	lw	a3,48(sp)
  a317c6:	5752                	lw	a4,52(sp)
  a317c8:	57e2                	lw	a5,56(sp)
  a317ca:	5872                	lw	a6,60(sp)
  a317cc:	4886                	lw	a7,64(sp)
  a317ce:	4916                	lw	s2,68(sp)
  a317d0:	49a6                	lw	s3,72(sp)
  a317d2:	4a36                	lw	s4,76(sp)
  a317d4:	4ac6                	lw	s5,80(sp)
  a317d6:	4b56                	lw	s6,84(sp)
  a317d8:	4be6                	lw	s7,88(sp)
  a317da:	4c76                	lw	s8,92(sp)
  a317dc:	5c86                	lw	s9,96(sp)
  a317de:	5d16                	lw	s10,100(sp)
  a317e0:	5da6                	lw	s11,104(sp)
  a317e2:	5e36                	lw	t3,108(sp)
  a317e4:	5ec6                	lw	t4,112(sp)
  a317e6:	5f56                	lw	t5,116(sp)
  a317e8:	5fe6                	lw	t6,120(sp)
  a317ea:	6109                	addi	sp,sp,128
  a317ec:	30200073          	mret

00a317f0 <default_interrupt1_handler>:
  a317f0:	30047073          	csrci	mstatus,8
  a317f4:	7119                	addi	sp,sp,-128
  a317f6:	c006                	sw	ra,0(sp)
  a317f8:	c20a                	sw	sp,4(sp)
  a317fa:	c40e                	sw	gp,8(sp)
  a317fc:	c612                	sw	tp,12(sp)
  a317fe:	c816                	sw	t0,16(sp)
  a31800:	ca1a                	sw	t1,20(sp)
  a31802:	cc1e                	sw	t2,24(sp)
  a31804:	ce22                	sw	s0,28(sp)
  a31806:	d026                	sw	s1,32(sp)
  a31808:	d22a                	sw	a0,36(sp)
  a3180a:	d42e                	sw	a1,40(sp)
  a3180c:	d632                	sw	a2,44(sp)
  a3180e:	d836                	sw	a3,48(sp)
  a31810:	da3a                	sw	a4,52(sp)
  a31812:	dc3e                	sw	a5,56(sp)
  a31814:	de42                	sw	a6,60(sp)
  a31816:	c0c6                	sw	a7,64(sp)
  a31818:	c2ca                	sw	s2,68(sp)
  a3181a:	c4ce                	sw	s3,72(sp)
  a3181c:	c6d2                	sw	s4,76(sp)
  a3181e:	c8d6                	sw	s5,80(sp)
  a31820:	cada                	sw	s6,84(sp)
  a31822:	ccde                	sw	s7,88(sp)
  a31824:	cee2                	sw	s8,92(sp)
  a31826:	d0e6                	sw	s9,96(sp)
  a31828:	d2ea                	sw	s10,100(sp)
  a3182a:	d4ee                	sw	s11,104(sp)
  a3182c:	d6f2                	sw	t3,108(sp)
  a3182e:	d8f6                	sw	t4,112(sp)
  a31830:	dafa                	sw	t5,116(sp)
  a31832:	dcfe                	sw	t6,120(sp)
  a31834:	341022f3          	csrr	t0,mepc
  a31838:	de96                	sw	t0,124(sp)
  a3183a:	34011073          	csrw	mscratch,sp
  a3183e:	fffd4117          	auipc	sp,0xfffd4
  a31842:	8c210113          	addi	sp,sp,-1854 # a05100 <__irq_stack_top__>
  a31846:	bfefb0ef          	jal	ra,a2cc44 <interrupt1_handler>
  a3184a:	34002173          	csrr	sp,mscratch
  a3184e:	6289                	lui	t0,0x2
  a31850:	88028293          	addi	t0,t0,-1920 # 1880 <ccause+0x8be>
  a31854:	3002a073          	csrs	mstatus,t0
  a31858:	52f6                	lw	t0,124(sp)
  a3185a:	34129073          	csrw	mepc,t0
  a3185e:	4082                	lw	ra,0(sp)
  a31860:	42c2                	lw	t0,16(sp)
  a31862:	4352                	lw	t1,20(sp)
  a31864:	43e2                	lw	t2,24(sp)
  a31866:	4472                	lw	s0,28(sp)
  a31868:	5482                	lw	s1,32(sp)
  a3186a:	5512                	lw	a0,36(sp)
  a3186c:	55a2                	lw	a1,40(sp)
  a3186e:	5632                	lw	a2,44(sp)
  a31870:	56c2                	lw	a3,48(sp)
  a31872:	5752                	lw	a4,52(sp)
  a31874:	57e2                	lw	a5,56(sp)
  a31876:	5872                	lw	a6,60(sp)
  a31878:	4886                	lw	a7,64(sp)
  a3187a:	4916                	lw	s2,68(sp)
  a3187c:	49a6                	lw	s3,72(sp)
  a3187e:	4a36                	lw	s4,76(sp)
  a31880:	4ac6                	lw	s5,80(sp)
  a31882:	4b56                	lw	s6,84(sp)
  a31884:	4be6                	lw	s7,88(sp)
  a31886:	4c76                	lw	s8,92(sp)
  a31888:	5c86                	lw	s9,96(sp)
  a3188a:	5d16                	lw	s10,100(sp)
  a3188c:	5da6                	lw	s11,104(sp)
  a3188e:	5e36                	lw	t3,108(sp)
  a31890:	5ec6                	lw	t4,112(sp)
  a31892:	5f56                	lw	t5,116(sp)
  a31894:	5fe6                	lw	t6,120(sp)
  a31896:	6109                	addi	sp,sp,128
  a31898:	30200073          	mret

00a3189c <default_interrupt2_handler>:
  a3189c:	30047073          	csrci	mstatus,8
  a318a0:	7119                	addi	sp,sp,-128
  a318a2:	c006                	sw	ra,0(sp)
  a318a4:	c20a                	sw	sp,4(sp)
  a318a6:	c40e                	sw	gp,8(sp)
  a318a8:	c612                	sw	tp,12(sp)
  a318aa:	c816                	sw	t0,16(sp)
  a318ac:	ca1a                	sw	t1,20(sp)
  a318ae:	cc1e                	sw	t2,24(sp)
  a318b0:	ce22                	sw	s0,28(sp)
  a318b2:	d026                	sw	s1,32(sp)
  a318b4:	d22a                	sw	a0,36(sp)
  a318b6:	d42e                	sw	a1,40(sp)
  a318b8:	d632                	sw	a2,44(sp)
  a318ba:	d836                	sw	a3,48(sp)
  a318bc:	da3a                	sw	a4,52(sp)
  a318be:	dc3e                	sw	a5,56(sp)
  a318c0:	de42                	sw	a6,60(sp)
  a318c2:	c0c6                	sw	a7,64(sp)
  a318c4:	c2ca                	sw	s2,68(sp)
  a318c6:	c4ce                	sw	s3,72(sp)
  a318c8:	c6d2                	sw	s4,76(sp)
  a318ca:	c8d6                	sw	s5,80(sp)
  a318cc:	cada                	sw	s6,84(sp)
  a318ce:	ccde                	sw	s7,88(sp)
  a318d0:	cee2                	sw	s8,92(sp)
  a318d2:	d0e6                	sw	s9,96(sp)
  a318d4:	d2ea                	sw	s10,100(sp)
  a318d6:	d4ee                	sw	s11,104(sp)
  a318d8:	d6f2                	sw	t3,108(sp)
  a318da:	d8f6                	sw	t4,112(sp)
  a318dc:	dafa                	sw	t5,116(sp)
  a318de:	dcfe                	sw	t6,120(sp)
  a318e0:	341022f3          	csrr	t0,mepc
  a318e4:	de96                	sw	t0,124(sp)
  a318e6:	34011073          	csrw	mscratch,sp
  a318ea:	fffd4117          	auipc	sp,0xfffd4
  a318ee:	81610113          	addi	sp,sp,-2026 # a05100 <__irq_stack_top__>
  a318f2:	b92fb0ef          	jal	ra,a2cc84 <interrupt2_handler>
  a318f6:	34002173          	csrr	sp,mscratch
  a318fa:	6289                	lui	t0,0x2
  a318fc:	88028293          	addi	t0,t0,-1920 # 1880 <ccause+0x8be>
  a31900:	3002a073          	csrs	mstatus,t0
  a31904:	52f6                	lw	t0,124(sp)
  a31906:	34129073          	csrw	mepc,t0
  a3190a:	4082                	lw	ra,0(sp)
  a3190c:	42c2                	lw	t0,16(sp)
  a3190e:	4352                	lw	t1,20(sp)
  a31910:	43e2                	lw	t2,24(sp)
  a31912:	4472                	lw	s0,28(sp)
  a31914:	5482                	lw	s1,32(sp)
  a31916:	5512                	lw	a0,36(sp)
  a31918:	55a2                	lw	a1,40(sp)
  a3191a:	5632                	lw	a2,44(sp)
  a3191c:	56c2                	lw	a3,48(sp)
  a3191e:	5752                	lw	a4,52(sp)
  a31920:	57e2                	lw	a5,56(sp)
  a31922:	5872                	lw	a6,60(sp)
  a31924:	4886                	lw	a7,64(sp)
  a31926:	4916                	lw	s2,68(sp)
  a31928:	49a6                	lw	s3,72(sp)
  a3192a:	4a36                	lw	s4,76(sp)
  a3192c:	4ac6                	lw	s5,80(sp)
  a3192e:	4b56                	lw	s6,84(sp)
  a31930:	4be6                	lw	s7,88(sp)
  a31932:	4c76                	lw	s8,92(sp)
  a31934:	5c86                	lw	s9,96(sp)
  a31936:	5d16                	lw	s10,100(sp)
  a31938:	5da6                	lw	s11,104(sp)
  a3193a:	5e36                	lw	t3,108(sp)
  a3193c:	5ec6                	lw	t4,112(sp)
  a3193e:	5f56                	lw	t5,116(sp)
  a31940:	5fe6                	lw	t6,120(sp)
  a31942:	6109                	addi	sp,sp,128
  a31944:	30200073          	mret

00a31948 <default_interrupt3_handler>:
  a31948:	30047073          	csrci	mstatus,8
  a3194c:	7119                	addi	sp,sp,-128
  a3194e:	c006                	sw	ra,0(sp)
  a31950:	c20a                	sw	sp,4(sp)
  a31952:	c40e                	sw	gp,8(sp)
  a31954:	c612                	sw	tp,12(sp)
  a31956:	c816                	sw	t0,16(sp)
  a31958:	ca1a                	sw	t1,20(sp)
  a3195a:	cc1e                	sw	t2,24(sp)
  a3195c:	ce22                	sw	s0,28(sp)
  a3195e:	d026                	sw	s1,32(sp)
  a31960:	d22a                	sw	a0,36(sp)
  a31962:	d42e                	sw	a1,40(sp)
  a31964:	d632                	sw	a2,44(sp)
  a31966:	d836                	sw	a3,48(sp)
  a31968:	da3a                	sw	a4,52(sp)
  a3196a:	dc3e                	sw	a5,56(sp)
  a3196c:	de42                	sw	a6,60(sp)
  a3196e:	c0c6                	sw	a7,64(sp)
  a31970:	c2ca                	sw	s2,68(sp)
  a31972:	c4ce                	sw	s3,72(sp)
  a31974:	c6d2                	sw	s4,76(sp)
  a31976:	c8d6                	sw	s5,80(sp)
  a31978:	cada                	sw	s6,84(sp)
  a3197a:	ccde                	sw	s7,88(sp)
  a3197c:	cee2                	sw	s8,92(sp)
  a3197e:	d0e6                	sw	s9,96(sp)
  a31980:	d2ea                	sw	s10,100(sp)
  a31982:	d4ee                	sw	s11,104(sp)
  a31984:	d6f2                	sw	t3,108(sp)
  a31986:	d8f6                	sw	t4,112(sp)
  a31988:	dafa                	sw	t5,116(sp)
  a3198a:	dcfe                	sw	t6,120(sp)
  a3198c:	341022f3          	csrr	t0,mepc
  a31990:	de96                	sw	t0,124(sp)
  a31992:	34011073          	csrw	mscratch,sp
  a31996:	fffd3117          	auipc	sp,0xfffd3
  a3199a:	76a10113          	addi	sp,sp,1898 # a05100 <__irq_stack_top__>
  a3199e:	b26fb0ef          	jal	ra,a2ccc4 <interrupt3_handler>
  a319a2:	34002173          	csrr	sp,mscratch
  a319a6:	6289                	lui	t0,0x2
  a319a8:	88028293          	addi	t0,t0,-1920 # 1880 <ccause+0x8be>
  a319ac:	3002a073          	csrs	mstatus,t0
  a319b0:	52f6                	lw	t0,124(sp)
  a319b2:	34129073          	csrw	mepc,t0
  a319b6:	4082                	lw	ra,0(sp)
  a319b8:	42c2                	lw	t0,16(sp)
  a319ba:	4352                	lw	t1,20(sp)
  a319bc:	43e2                	lw	t2,24(sp)
  a319be:	4472                	lw	s0,28(sp)
  a319c0:	5482                	lw	s1,32(sp)
  a319c2:	5512                	lw	a0,36(sp)
  a319c4:	55a2                	lw	a1,40(sp)
  a319c6:	5632                	lw	a2,44(sp)
  a319c8:	56c2                	lw	a3,48(sp)
  a319ca:	5752                	lw	a4,52(sp)
  a319cc:	57e2                	lw	a5,56(sp)
  a319ce:	5872                	lw	a6,60(sp)
  a319d0:	4886                	lw	a7,64(sp)
  a319d2:	4916                	lw	s2,68(sp)
  a319d4:	49a6                	lw	s3,72(sp)
  a319d6:	4a36                	lw	s4,76(sp)
  a319d8:	4ac6                	lw	s5,80(sp)
  a319da:	4b56                	lw	s6,84(sp)
  a319dc:	4be6                	lw	s7,88(sp)
  a319de:	4c76                	lw	s8,92(sp)
  a319e0:	5c86                	lw	s9,96(sp)
  a319e2:	5d16                	lw	s10,100(sp)
  a319e4:	5da6                	lw	s11,104(sp)
  a319e6:	5e36                	lw	t3,108(sp)
  a319e8:	5ec6                	lw	t4,112(sp)
  a319ea:	5f56                	lw	t5,116(sp)
  a319ec:	5fe6                	lw	t6,120(sp)
  a319ee:	6109                	addi	sp,sp,128
  a319f0:	30200073          	mret

00a319f4 <default_interrupt4_handler>:
  a319f4:	30047073          	csrci	mstatus,8
  a319f8:	7119                	addi	sp,sp,-128
  a319fa:	c006                	sw	ra,0(sp)
  a319fc:	c20a                	sw	sp,4(sp)
  a319fe:	c40e                	sw	gp,8(sp)
  a31a00:	c612                	sw	tp,12(sp)
  a31a02:	c816                	sw	t0,16(sp)
  a31a04:	ca1a                	sw	t1,20(sp)
  a31a06:	cc1e                	sw	t2,24(sp)
  a31a08:	ce22                	sw	s0,28(sp)
  a31a0a:	d026                	sw	s1,32(sp)
  a31a0c:	d22a                	sw	a0,36(sp)
  a31a0e:	d42e                	sw	a1,40(sp)
  a31a10:	d632                	sw	a2,44(sp)
  a31a12:	d836                	sw	a3,48(sp)
  a31a14:	da3a                	sw	a4,52(sp)
  a31a16:	dc3e                	sw	a5,56(sp)
  a31a18:	de42                	sw	a6,60(sp)
  a31a1a:	c0c6                	sw	a7,64(sp)
  a31a1c:	c2ca                	sw	s2,68(sp)
  a31a1e:	c4ce                	sw	s3,72(sp)
  a31a20:	c6d2                	sw	s4,76(sp)
  a31a22:	c8d6                	sw	s5,80(sp)
  a31a24:	cada                	sw	s6,84(sp)
  a31a26:	ccde                	sw	s7,88(sp)
  a31a28:	cee2                	sw	s8,92(sp)
  a31a2a:	d0e6                	sw	s9,96(sp)
  a31a2c:	d2ea                	sw	s10,100(sp)
  a31a2e:	d4ee                	sw	s11,104(sp)
  a31a30:	d6f2                	sw	t3,108(sp)
  a31a32:	d8f6                	sw	t4,112(sp)
  a31a34:	dafa                	sw	t5,116(sp)
  a31a36:	dcfe                	sw	t6,120(sp)
  a31a38:	341022f3          	csrr	t0,mepc
  a31a3c:	de96                	sw	t0,124(sp)
  a31a3e:	34011073          	csrw	mscratch,sp
  a31a42:	fffd3117          	auipc	sp,0xfffd3
  a31a46:	6be10113          	addi	sp,sp,1726 # a05100 <__irq_stack_top__>
  a31a4a:	abafb0ef          	jal	ra,a2cd04 <interrupt4_handler>
  a31a4e:	34002173          	csrr	sp,mscratch
  a31a52:	6289                	lui	t0,0x2
  a31a54:	88028293          	addi	t0,t0,-1920 # 1880 <ccause+0x8be>
  a31a58:	3002a073          	csrs	mstatus,t0
  a31a5c:	52f6                	lw	t0,124(sp)
  a31a5e:	34129073          	csrw	mepc,t0
  a31a62:	4082                	lw	ra,0(sp)
  a31a64:	42c2                	lw	t0,16(sp)
  a31a66:	4352                	lw	t1,20(sp)
  a31a68:	43e2                	lw	t2,24(sp)
  a31a6a:	4472                	lw	s0,28(sp)
  a31a6c:	5482                	lw	s1,32(sp)
  a31a6e:	5512                	lw	a0,36(sp)
  a31a70:	55a2                	lw	a1,40(sp)
  a31a72:	5632                	lw	a2,44(sp)
  a31a74:	56c2                	lw	a3,48(sp)
  a31a76:	5752                	lw	a4,52(sp)
  a31a78:	57e2                	lw	a5,56(sp)
  a31a7a:	5872                	lw	a6,60(sp)
  a31a7c:	4886                	lw	a7,64(sp)
  a31a7e:	4916                	lw	s2,68(sp)
  a31a80:	49a6                	lw	s3,72(sp)
  a31a82:	4a36                	lw	s4,76(sp)
  a31a84:	4ac6                	lw	s5,80(sp)
  a31a86:	4b56                	lw	s6,84(sp)
  a31a88:	4be6                	lw	s7,88(sp)
  a31a8a:	4c76                	lw	s8,92(sp)
  a31a8c:	5c86                	lw	s9,96(sp)
  a31a8e:	5d16                	lw	s10,100(sp)
  a31a90:	5da6                	lw	s11,104(sp)
  a31a92:	5e36                	lw	t3,108(sp)
  a31a94:	5ec6                	lw	t4,112(sp)
  a31a96:	5f56                	lw	t5,116(sp)
  a31a98:	5fe6                	lw	t6,120(sp)
  a31a9a:	6109                	addi	sp,sp,128
  a31a9c:	30200073          	mret

00a31aa0 <default_interrupt5_handler>:
  a31aa0:	30047073          	csrci	mstatus,8
  a31aa4:	7119                	addi	sp,sp,-128
  a31aa6:	c006                	sw	ra,0(sp)
  a31aa8:	c20a                	sw	sp,4(sp)
  a31aaa:	c40e                	sw	gp,8(sp)
  a31aac:	c612                	sw	tp,12(sp)
  a31aae:	c816                	sw	t0,16(sp)
  a31ab0:	ca1a                	sw	t1,20(sp)
  a31ab2:	cc1e                	sw	t2,24(sp)
  a31ab4:	ce22                	sw	s0,28(sp)
  a31ab6:	d026                	sw	s1,32(sp)
  a31ab8:	d22a                	sw	a0,36(sp)
  a31aba:	d42e                	sw	a1,40(sp)
  a31abc:	d632                	sw	a2,44(sp)
  a31abe:	d836                	sw	a3,48(sp)
  a31ac0:	da3a                	sw	a4,52(sp)
  a31ac2:	dc3e                	sw	a5,56(sp)
  a31ac4:	de42                	sw	a6,60(sp)
  a31ac6:	c0c6                	sw	a7,64(sp)
  a31ac8:	c2ca                	sw	s2,68(sp)
  a31aca:	c4ce                	sw	s3,72(sp)
  a31acc:	c6d2                	sw	s4,76(sp)
  a31ace:	c8d6                	sw	s5,80(sp)
  a31ad0:	cada                	sw	s6,84(sp)
  a31ad2:	ccde                	sw	s7,88(sp)
  a31ad4:	cee2                	sw	s8,92(sp)
  a31ad6:	d0e6                	sw	s9,96(sp)
  a31ad8:	d2ea                	sw	s10,100(sp)
  a31ada:	d4ee                	sw	s11,104(sp)
  a31adc:	d6f2                	sw	t3,108(sp)
  a31ade:	d8f6                	sw	t4,112(sp)
  a31ae0:	dafa                	sw	t5,116(sp)
  a31ae2:	dcfe                	sw	t6,120(sp)
  a31ae4:	341022f3          	csrr	t0,mepc
  a31ae8:	de96                	sw	t0,124(sp)
  a31aea:	34011073          	csrw	mscratch,sp
  a31aee:	fffd3117          	auipc	sp,0xfffd3
  a31af2:	61210113          	addi	sp,sp,1554 # a05100 <__irq_stack_top__>
  a31af6:	a4efb0ef          	jal	ra,a2cd44 <interrupt5_handler>
  a31afa:	34002173          	csrr	sp,mscratch
  a31afe:	6289                	lui	t0,0x2
  a31b00:	88028293          	addi	t0,t0,-1920 # 1880 <ccause+0x8be>
  a31b04:	3002a073          	csrs	mstatus,t0
  a31b08:	52f6                	lw	t0,124(sp)
  a31b0a:	34129073          	csrw	mepc,t0
  a31b0e:	4082                	lw	ra,0(sp)
  a31b10:	42c2                	lw	t0,16(sp)
  a31b12:	4352                	lw	t1,20(sp)
  a31b14:	43e2                	lw	t2,24(sp)
  a31b16:	4472                	lw	s0,28(sp)
  a31b18:	5482                	lw	s1,32(sp)
  a31b1a:	5512                	lw	a0,36(sp)
  a31b1c:	55a2                	lw	a1,40(sp)
  a31b1e:	5632                	lw	a2,44(sp)
  a31b20:	56c2                	lw	a3,48(sp)
  a31b22:	5752                	lw	a4,52(sp)
  a31b24:	57e2                	lw	a5,56(sp)
  a31b26:	5872                	lw	a6,60(sp)
  a31b28:	4886                	lw	a7,64(sp)
  a31b2a:	4916                	lw	s2,68(sp)
  a31b2c:	49a6                	lw	s3,72(sp)
  a31b2e:	4a36                	lw	s4,76(sp)
  a31b30:	4ac6                	lw	s5,80(sp)
  a31b32:	4b56                	lw	s6,84(sp)
  a31b34:	4be6                	lw	s7,88(sp)
  a31b36:	4c76                	lw	s8,92(sp)
  a31b38:	5c86                	lw	s9,96(sp)
  a31b3a:	5d16                	lw	s10,100(sp)
  a31b3c:	5da6                	lw	s11,104(sp)
  a31b3e:	5e36                	lw	t3,108(sp)
  a31b40:	5ec6                	lw	t4,112(sp)
  a31b42:	5f56                	lw	t5,116(sp)
  a31b44:	5fe6                	lw	t6,120(sp)
  a31b46:	6109                	addi	sp,sp,128
  a31b48:	30200073          	mret

00a31b4c <default_local_interrupt_handler>:
  a31b4c:	30047073          	csrci	mstatus,8
  a31b50:	7119                	addi	sp,sp,-128
  a31b52:	c006                	sw	ra,0(sp)
  a31b54:	c20a                	sw	sp,4(sp)
  a31b56:	c40e                	sw	gp,8(sp)
  a31b58:	c612                	sw	tp,12(sp)
  a31b5a:	c816                	sw	t0,16(sp)
  a31b5c:	ca1a                	sw	t1,20(sp)
  a31b5e:	cc1e                	sw	t2,24(sp)
  a31b60:	ce22                	sw	s0,28(sp)
  a31b62:	d026                	sw	s1,32(sp)
  a31b64:	d22a                	sw	a0,36(sp)
  a31b66:	d42e                	sw	a1,40(sp)
  a31b68:	d632                	sw	a2,44(sp)
  a31b6a:	d836                	sw	a3,48(sp)
  a31b6c:	da3a                	sw	a4,52(sp)
  a31b6e:	dc3e                	sw	a5,56(sp)
  a31b70:	de42                	sw	a6,60(sp)
  a31b72:	c0c6                	sw	a7,64(sp)
  a31b74:	c2ca                	sw	s2,68(sp)
  a31b76:	c4ce                	sw	s3,72(sp)
  a31b78:	c6d2                	sw	s4,76(sp)
  a31b7a:	c8d6                	sw	s5,80(sp)
  a31b7c:	cada                	sw	s6,84(sp)
  a31b7e:	ccde                	sw	s7,88(sp)
  a31b80:	cee2                	sw	s8,92(sp)
  a31b82:	d0e6                	sw	s9,96(sp)
  a31b84:	d2ea                	sw	s10,100(sp)
  a31b86:	d4ee                	sw	s11,104(sp)
  a31b88:	d6f2                	sw	t3,108(sp)
  a31b8a:	d8f6                	sw	t4,112(sp)
  a31b8c:	dafa                	sw	t5,116(sp)
  a31b8e:	dcfe                	sw	t6,120(sp)
  a31b90:	341022f3          	csrr	t0,mepc
  a31b94:	de96                	sw	t0,124(sp)
  a31b96:	34011073          	csrw	mscratch,sp
  a31b9a:	fffd3117          	auipc	sp,0xfffd3
  a31b9e:	56610113          	addi	sp,sp,1382 # a05100 <__irq_stack_top__>
  a31ba2:	9e2fb0ef          	jal	ra,a2cd84 <local_interrupt_handler>
  a31ba6:	34002173          	csrr	sp,mscratch
  a31baa:	6289                	lui	t0,0x2
  a31bac:	88028293          	addi	t0,t0,-1920 # 1880 <ccause+0x8be>
  a31bb0:	3002a073          	csrs	mstatus,t0
  a31bb4:	52f6                	lw	t0,124(sp)
  a31bb6:	34129073          	csrw	mepc,t0
  a31bba:	4082                	lw	ra,0(sp)
  a31bbc:	42c2                	lw	t0,16(sp)
  a31bbe:	4352                	lw	t1,20(sp)
  a31bc0:	43e2                	lw	t2,24(sp)
  a31bc2:	4472                	lw	s0,28(sp)
  a31bc4:	5482                	lw	s1,32(sp)
  a31bc6:	5512                	lw	a0,36(sp)
  a31bc8:	55a2                	lw	a1,40(sp)
  a31bca:	5632                	lw	a2,44(sp)
  a31bcc:	56c2                	lw	a3,48(sp)
  a31bce:	5752                	lw	a4,52(sp)
  a31bd0:	57e2                	lw	a5,56(sp)
  a31bd2:	5872                	lw	a6,60(sp)
  a31bd4:	4886                	lw	a7,64(sp)
  a31bd6:	4916                	lw	s2,68(sp)
  a31bd8:	49a6                	lw	s3,72(sp)
  a31bda:	4a36                	lw	s4,76(sp)
  a31bdc:	4ac6                	lw	s5,80(sp)
  a31bde:	4b56                	lw	s6,84(sp)
  a31be0:	4be6                	lw	s7,88(sp)
  a31be2:	4c76                	lw	s8,92(sp)
  a31be4:	5c86                	lw	s9,96(sp)
  a31be6:	5d16                	lw	s10,100(sp)
  a31be8:	5da6                	lw	s11,104(sp)
  a31bea:	5e36                	lw	t3,108(sp)
  a31bec:	5ec6                	lw	t4,112(sp)
  a31bee:	5f56                	lw	t5,116(sp)
  a31bf0:	5fe6                	lw	t6,120(sp)
  a31bf2:	6109                	addi	sp,sp,128
  a31bf4:	30200073          	mret

00a31bf8 <global_interrupt_lock>:
  a31bf8:	30002573          	csrr	a0,mstatus
  a31bfc:	42a1                	li	t0,8
  a31bfe:	3002b073          	csrc	mstatus,t0
  a31c02:	8082                	ret

00a31c04 <global_interrupt_unlock>:
  a31c04:	30002573          	csrr	a0,mstatus
  a31c08:	42a1                	li	t0,8
  a31c0a:	3002a073          	csrs	mstatus,t0
  a31c0e:	8082                	ret

00a31c10 <global_interrupt_restore>:
  a31c10:	30051073          	csrw	mstatus,a0
  a31c14:	8082                	ret
  a31c16:	0000                	unimp
  a31c18:	ffff                	.2byte	0xffff
  a31c1a:	00ff                	.2byte	0xff
  a31c1c:	ffff                	.2byte	0xffff
  a31c1e:	00ff                	.2byte	0xff

00a31c20 <__clz_tab>:
  a31c20:	0100 0202 0303 0303 0404 0404 0404 0404     ................
  a31c30:	0505 0505 0505 0505 0505 0505 0505 0505     ................
  a31c40:	0606 0606 0606 0606 0606 0606 0606 0606     ................
  a31c50:	0606 0606 0606 0606 0606 0606 0606 0606     ................
  a31c60:	0707 0707 0707 0707 0707 0707 0707 0707     ................
  a31c70:	0707 0707 0707 0707 0707 0707 0707 0707     ................
  a31c80:	0707 0707 0707 0707 0707 0707 0707 0707     ................
  a31c90:	0707 0707 0707 0707 0707 0707 0707 0707     ................
  a31ca0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
  a31cb0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
  a31cc0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
  a31cd0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
  a31ce0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
  a31cf0:	0808 0808 0808 0808 0808 0808 0808 0808     ................
  a31d00:	0808 0808 0808 0808 0808 0808 0808 0808     ................
  a31d10:	0808 0808 0808 0808 0808 0808 0808 0808     ................

00a31d20 <sfc_cfg>:
  a31d20:	0104 0000 0000 0020 0000 0080 6c46 7361     ...... .....Flas
  a31d30:	6268 6f6f 2074 6155 7472 4920 696e 2074     hboot Uart Init 
  a31d40:	7553 6363 0021 0000 6c46 7361 6268 6f6f     Succ!...Flashboo
  a31d50:	2074 614d 6c6c 636f 4920 696e 2074 7553     t Malloc Init Su
  a31d60:	6363 0021 6c46 7361 2068 6e49 7469 4620     cc!.Flash Init F
  a31d70:	6961 216c 7220 7465 3d20 0020 6c46 7361     ail! ret = .Flas
  a31d80:	2068 6e49 7469 5320 6375 2163 0000 0000     h Init Succ!....
  a31d90:	4653 2043 6966 2078 5253 7220 7465 3d20     SFC fix SR ret =
  a31da0:	0000 0000 2e31 3031 312e 3130 0000 0000     ....1.10.101....
  a31db0:	6c66 7361 6268 6f6f 2074 6576 7372 6f69     flashboot versio
  a31dc0:	206e 203a 7325 0a0d 0000 0000 6175 6970     n : %s......uapi
  a31dd0:	705f 7261 6974 6974 6e6f 675f 7465 695f     _partition_get_i
  a31de0:	666e 206f 6166 6c69 6465 0020 7075 5f67     nfo failed .upg_
  a31df0:	6c66 7361 5f68 6572 6461 6620 6961 656c     flash_read faile
  a31e00:	2064 0000 6f4e 6e20 6565 2064 6f74 7520     d ..No need to u
  a31e10:	6770 6172 6564 2e2e 002e 0000 656e 6465     pgrade......need
  a31e20:	7520 6770 6172 6564 0000 0000 2d2d 2d2d      upgrade....----
  a31e30:	2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d 2d2d     ----------------
  a31e40:	2d2d 2d2d 2d2d 0000 7075 7267 6461 2065     ------..upgrade 
  a31e50:	6166 6c69 6465 202c 6572 6573 2074 6f6e     failed, reset no
  a31e60:	0077 0000 7075 7267 6461 2065 7573 6363     w...upgrade succ
  a31e70:	7365 2c73 7220 7365 7465 6e20 776f 0000     ess, reset now..
  a31e80:	6c46 7361 6268 6f6f 2074 6567 2074 7061     Flashboot get ap
  a31e90:	2070 6170 7472 7469 6f69 206e 6166 6c69     p partition fail
  a31ea0:	6465 2c21 6220 6f6f 2074 6261 726f 2174     ed!, boot abort!
  a31eb0:	0000 0000 6c66 7361 5f68 6e65 7263 7079     ....flash_encryp
  a31ec0:	2074 6964 6173 6c62 2e65 0000 7061 5f70     t disable...app_
  a31ed0:	6d69 6761 2065 7473 7261 2074 726f 6520     image start or e
  a31ee0:	646e 6120 6464 2072 7265 2c72 6d20 7375     nd addr err, mus
  a31ef0:	2074 3532 6236 7479 2065 6c61 6769 6d6e     t 256byte alignm
  a31f00:	6e65 2074 0000 0000 6c66 7361 5f68 6e65     ent ....flash_en
  a31f10:	7263 7079 2074 6e65 6261 656c 002e 0000     crypt enable....
  a31f20:	6166 6370 735f 7465 635f 6e6f 6966 2067     fapc_set_config 
  a31f30:	7264 5f76 6f72 5f6d 6963 6870 7265 635f     drv_rom_cipher_c
  a31f40:	6e6f 6966 5f67 646f 6b72 2031 7265 2072     onfig_odrk1 err 
  a31f50:	203d 0000 6166 6370 735f 7465 635f 6e6f     = ..fapc_set_con
  a31f60:	6966 2067 7264 5f76 6f72 5f6d 6963 6870     fig drv_rom_ciph
  a31f70:	7265 665f 7061 5f63 6f63 666e 6769 6520     er_fapc_config e
  a31f80:	7272 3d20 0020 0000 6166 6370 735f 7465     rr = ...fapc_set
  a31f90:	635f 6e6f 6966 2067 7264 5f76 6f72 5f6d     _config drv_rom_
  a31fa0:	6963 6870 7265 665f 7061 5f63 7962 6170     cipher_fapc_bypa
  a31fb0:	7373 635f 6e6f 6966 2067 7265 2072 203d     ss_config err = 
  a31fc0:	0000 0000 6c66 7361 6268 6f6f 2074 6576     ....flashboot ve
  a31fd0:	6972 7966 695f 616d 6567 615f 7070 685f     rify_image_app_h
  a31fe0:	6165 2064 6166 6c69 6465 2121 7220 7465     ead failed!! ret
  a31ff0:	3d20 0020 6576 6972 7966 695f 616d 6567      = .verify_image
  a32000:	615f 7070 625f 646f 2079 6166 6c69 6465     _app_body failed
  a32010:	2121 7220 7465 3d20 0020 0000 6572 6573     !! ret = ...rese
  a32020:	5f74 7075 7267 6461 5f65 6c66 6761 6620     t_upgrade_flag f
  a32030:	6961 2c6c 7220 7465 3d20 0020 6572 7571     ail, ret = .requ
  a32040:	7365 5f74 7075 7267 6461 2065 6166 6c69     est_upgrade fail
  a32050:	202c 6f66 6174 705f 746b 6e5f 746f 655f     , fota_pkt_not_e
  a32060:	6978 2e74 0000 0000 6f66 6174 705f 746b     xit.....fota_pkt
  a32070:	6520 6978 2c74 7420 7972 665f 746f 5f61      exit, try_fota_
  a32080:	6966 5f78 7061 2e70 0000 0000 6c46 7361     fix_app.....Flas
  a32090:	6268 6f6f 2074 6162 6b63 7075 6920 2073     hboot backup is 
  a320a0:	6f77 6b72 6e69 2167 0000 0000 6c46 7361     working!....Flas
  a320b0:	6268 6f6f 2074 6170 7472 7469 6f69 206e     hboot partition 
  a320c0:	6e69 6f66 6720 7465 6620 6961 216c 0000     info get fail!..
  a320d0:	6c66 7361 6268 6f6f 2074 6572 6f63 6576     flashboot recove
  a320e0:	7972 6520 6172 6573 6620 6961 656c 2164     ry erase failed!
  a320f0:	2021 6572 2074 203d 0000 0000 6c66 7361     ! ret = ....flas
  a32100:	6268 6f6f 2074 6572 6f63 6576 7972 7720     hboot recovery w
  a32110:	6972 6574 6620 6961 656c 2164 2021 6572     rite failed!! re
  a32120:	2074 203d 0000 0000 6c46 7361 6268 6f6f     t = ....Flashboo
  a32130:	2074 6966 2078 6b6f 0021 0000 6964 5f65     t fix ok!...die_
  a32140:	6469 6520 7566 6573 725f 6165 5f64 7469     id efuse_read_it
  a32150:	6d65 6620 6961 216c 0000 0000 6964 2065     em fail!....die 
  a32160:	6469 6d20 6d65 6d63 2070 6166 6c69 0021     id memcmp fail!.
  a32170:	6d69 6761 2065 6469 6520 7272 726f 0021     image id error!.
  a32180:	6576 7372 6f69 206e 6665 7375 5f65 6572     version efuse_re
  a32190:	6461 695f 6574 206d 6166 6c69 0021 0000     ad_item fail!...
  a321a0:	736d 6469 6320 6d6f 6170 6572 6620 6961     msid compare fai
  a321b0:	216c 0000 6665 7375 5f65 6572 6461 695f     l!..efuse_read_i
  a321c0:	6574 206d 6576 6972 7966 6520 616e 6c62     tem verify enabl
  a321d0:	2065 6166 6c69 0021 6576 7372 6f69 206e     e fail!.version 
  a321e0:	6f63 706d 7261 2065 6166 6c69 0021 0000     compare fail!...

00a321f0 <g_verify_table>:
  a321f0:	0000 0000 f01e 4bd2 0000 0000 0100 0000     .......K........
  a32200:	f02d 4bd2 0000 0000 0002 0000 3c1e 4b1e     -..K.........<.K
  a32210:	0003 0000 0102 0000 3c2d 4b1e 0003 0000     ........-<.K....
  a32220:	0003 0000 871e 4b69 0003 0000 0103 0000     ......iK........
  a32230:	872d 4b69 0003 0000 0004 0000 3c1e 4b1e     -.iK.........<.K
  a32240:	0003 0000 0104 0000 3c2d 4b1e 0003 0000     ........-<.K....
  a32250:	0005 0000 871e 4b69 0003 0000 0105 0000     ......iK........
  a32260:	872d 4b69 0003 0000 0201 0000 a51e 4b87     -.iK...........K
  a32270:	0004 0000 0301 0000 a52d 4b87 0004 0000     ........-..K....
  a32280:	0006 0000 2d1e 4b0f 0002 0000 0106 0000     .....-.K........
  a32290:	2d2d 4b0f 0002 0000 6573 7563 6572 615f     --.K....secure_a
  a322a0:	7475 6568 746e 6369 7461 2065 7264 5f76     uthenticate drv_
  a322b0:	6f72 5f6d 6963 6870 7265 735f 6168 3532     rom_cipher_sha25
  a322c0:	2036 6166 6c69 0021 6573 7563 6572 615f     6 fail!.secure_a
  a322d0:	7475 6568 746e 6369 7461 2065 7264 5f76     uthenticate drv_
  a322e0:	6f72 5f6d 6963 6870 7265 705f 656b 625f     rom_cipher_pke_b
  a322f0:	3270 3635 5f72 6576 6972 7966 6620 6961     p256r_verify fai
  a32300:	216c 0000 6576 6972 7966 695f 616d 6567     l!..verify_image
  a32310:	635f 646f 5f65 7261 6165 6420 7672 725f     _code_area drv_r
  a32320:	6d6f 635f 7069 6568 5f72 6873 3261 3635     om_cipher_sha256
  a32330:	0021 0000 6576 6972 7966 695f 616d 6567     !...verify_image
  a32340:	635f 646f 5f65 7261 6165 6820 7361 2068     _code_area hash 
  a32350:	656d 636d 706d 6620 6961 216c 0000 0000     memcmp fail!....
  a32360:	6576 6972 7966 695f 616d 6567 635f 646f     verify_image_cod
  a32370:	5f65 7261 6165 6620 6961 216c 0000 0000     e_area fail!....
  a32380:	6576 6972 7966 695f 616d 6567 6b5f 7965     verify_image_key
  a32390:	615f 6572 2061 6573 7563 6572 7620 7265     _area secure ver
  a323a0:	6669 2079 6964 6173 6c62 2165 0000 0000     ify disable!....
  a323b0:	6576 6972 7966 695f 616d 6567 6b5f 7965     verify_image_key
  a323c0:	615f 6572 2061 6573 7563 6572 7620 7265     _area secure ver
  a323d0:	6669 2079 7265 6f72 2172 0000 6576 6972     ify error!..veri
  a323e0:	7966 695f 616d 6567 6b5f 7965 615f 6572     fy_image_key_are
  a323f0:	2061 6f72 746f 656b 5f79 7473 7461 7375     a rootkey_status
  a32400:	6920 766e 6c61 6469 0021 0000 6576 6972      invalid!...veri
  a32410:	7966 695f 616d 6567 6b5f 7965 615f 6572     fy_image_key_are
  a32420:	2061 6d69 6761 2065 6469 6520 7272 726f     a image id error
  a32430:	0021 0000 6576 6972 7966 695f 616d 6567     !...verify_image
  a32440:	6b5f 7965 615f 6572 2061 6573 7563 6572     _key_area secure
  a32450:	615f 7475 6568 746e 6369 7461 2165 0000     _authenticate!..
  a32460:	6576 6972 7966 695f 616d 6567 6b5f 7965     verify_image_key
  a32470:	615f 6572 2061 656b 2079 6576 7372 6f69     _area key versio
  a32480:	206e 7265 6f72 2172 0000 0000 6576 6972     n error!....veri
  a32490:	7966 695f 616d 6567 6b5f 7965 615f 6572     fy_image_key_are
  a324a0:	2061 736d 6469 6520 7272 726f 0021 0000     a msid error!...
  a324b0:	6576 6972 7966 695f 616d 6567 6b5f 7965     verify_image_key
  a324c0:	615f 6572 2061 6964 2065 6469 6520 7272     _area die id err
  a324d0:	726f 0021 6576 6972 7966 695f 616d 6567     or!.verify_image
  a324e0:	6b5f 7965 615f 6572 2061 6166 6c69 0021     _key_area fail!.
  a324f0:	6576 6972 7966 695f 616d 6567 635f 646f     verify_image_cod
  a32500:	5f65 6e69 6f66 7320 6365 7275 2065 6576     e_info secure ve
  a32510:	6972 7966 6420 7369 6261 656c 0021 0000     rify disable!...
  a32520:	6576 6972 7966 695f 616d 6567 635f 646f     verify_image_cod
  a32530:	5f65 6e69 6f66 7320 6365 7275 2065 6576     e_info secure ve
  a32540:	6972 7966 6520 7272 726f 0021 6576 6972     rify error!.veri
  a32550:	7966 695f 616d 6567 635f 646f 5f65 6e69     fy_image_code_in
  a32560:	6f66 6920 616d 6567 6920 2064 7265 6f72     fo image id erro
  a32570:	2172 0000 6576 6972 7966 695f 616d 6567     r!..verify_image
  a32580:	635f 646f 5f65 6e69 6f66 7320 6365 7275     _code_info secur
  a32590:	5f65 7561 6874 6e65 6974 6163 6574 0021     e_authenticate!.
  a325a0:	6576 6972 7966 695f 616d 6567 635f 646f     verify_image_cod
  a325b0:	5f65 6e69 6f66 6b20 7965 7620 7265 6973     e_info key versi
  a325c0:	6e6f 6520 7272 726f 0021 0000 6576 6972     on error!...veri
  a325d0:	7966 695f 616d 6567 635f 646f 5f65 6e69     fy_image_code_in
  a325e0:	6f66 6d20 6973 2064 7265 6f72 2172 0000     fo msid error!..
  a325f0:	6576 6972 7966 695f 616d 6567 635f 646f     verify_image_cod
  a32600:	5f65 6e69 6f66 6620 6961 216c 0000 0000     e_info fail!....

00a32610 <g_sha256_ival>:
  a32610:	096a 67e6 67bb 85ae 6e3c 72f3 4fa5 3af5     j..g.g..<n.r.O.:
  a32620:	0e51 7f52 059b 8c68 831f abd9 e05b 19cd     Q.R...h.....[...

00a32630 <g_sm3_ival>:
  a32630:	8073 6f16 1449 b9b2 2417 d742 8ada 0006     s..oI....$B.....
  a32640:	6fa9 bc30 3116 aa38 8de3 4dee fbb0 4e0e     .o0..18....M...N
  a32650:	a43c 00a2 a3fa 00a2 a4d6 00a2 a578 00a2     <...........x...
  a32660:	a592 00a2 b7b0 00a2 b79a 00a2 b792 00a2     ................
  a32670:	b79a 00a2 b79e 00a2 b79a 00a2 b7a8 00a2     ................
  a32680:	b79a 00a2 b79a 00a2 b79a 00a2 b79a 00a2     ................
  a32690:	b79a 00a2 b79e 00a2 b7a8 00a2               ............

00a3269c <CSWTCH.7>:
  a3269c:	1001 0020                                   .. .

00a326a0 <g_hal_uart_ctrl_func_array>:
  a326a0:	b84e 00a2 0000 0000 b7e4 00a2 b7d6 00a2     N...............
  a326b0:	b7c8 00a2 b7ba 00a2 b7ba 00a2 b590 00a2     ................
  a326c0:	b5a6 00a2 b5dc 00a2 0000 0000 b6ba 00a2     ................
  a326d0:	b69a 00a2 0000 0000 0000 0000 0000 0000     ................
  a326e0:	0000 0000 b5be 00a2 b532 00a2 b544 00a2     ........2...D...
  a326f0:	0000 0000                                   ....

00a326f4 <g_uart_base_addrs>:
  a326f4:	0000 4401 1000 4401 2000 4401               ...D...D. .D

00a32700 <g_default_erase_cmds>:
  a32700:	c639 ffff 06c1 4000 0101 0400               9......@....

00a3270c <g_default_quad_enable>:
  a3270c:	0002 0000 0000 0000                         ........

00a32714 <g_default_read_cmds>:
  a32714:	0019 0000                                   ....

00a32718 <g_default_write_cmds>:
  a32718:	0001 0000 0011 0000                         ........

00a32720 <g_flash_common_erase_cmds>:
  a32720:	c639 ffff 06c1 4000 0291 2000 0101 0400     9......@... ....

00a32730 <g_flash_common_read_cmds>:
  a32730:	0019 0000 4059 0000 49d9 0000 55d9 0000     ....Y@...I...U..
  a32740:	6b59 0000 f759 0000                         Yk..Y...

00a32748 <g_flash_common_write_cmds>:
  a32748:	0001 0000 0011 0000 4000 0000 4000 0000     .........@...@..
  a32758:	2991 0000 4000 0000                         .)...@..

00a32760 <g_flash_gd_bus_enable>:
  a32760:	0301 0005 0000 0301 0135 0001 0002 0000     ........5.......
  a32770:	0000 0000                                   ....

00a32774 <g_flash_spi_info_list>:
  a32774:	40c8 0016 0000 0040 0004 0000 2730 00a3     .@....@.....0'..
  a32784:	2748 00a3 2720 00a3 2760 00a3               H'.. '..`'..

00a32790 <g_flash_spi_unknown_info>:
  a32790:	ffff 00ff 0000 0008 0003 0000 2714 00a3     .............'..
  a327a0:	2718 00a3 2700 00a3 270c 00a3 6f4e 6e20     .'...'...'..No n
  a327b0:	6565 2064 6f74 6620 7869 5320 2152 0a0d     eed to fix SR!..
  a327c0:	0000 0000 5253 6620 7869 6f20 216b 0a0d     ....SR fix ok!..
  a327d0:	0000 0000 5253 6425 305b 2578 5d78 6e20     ....SR%d[0x%x] n
  a327e0:	6565 7364 6620 7869 6e69 2c67 6520 7078     eeds fixing, exp
  a327f0:	6365 5b74 7830 7825 0d5d 000a               ect[0x%x]...

00a327fc <g_efuse_cfg>:
  a327fc:	0000 0008 0003 0008 00a0 0003 0270 0020     ............p. .
  a3280c:	0003 0290 0008 0003 0298 0008 0003 02a0     ................
  a3281c:	0100 0003 03a0 0020 0003 03c0 0001 0003     ...... .........
  a3282c:	03e8 0008 0003 0630 0030 0003 0660 0030     ......0.0...`.0.
  a3283c:	0003 0690 0030 0003 06c0 0030 0003 06f0     ....0.....0.....
  a3284c:	0080 0003 02a0 0100 0003 046f 0001 0003     ..........o.....
  a3285c:	0460 000f 0003 0470 0010 0003 7830 0000     `.....p.....0x..
  a3286c:	7075 6164 6574 725f 7365 7465 635f 756f     update_reset_cou
  a3287c:	746e 0020 4f52 7c4d 6564 6166 6c75 5f74     nt .ROM|default_
  a3288c:	6168 646e 656c 2072 203a 6e69 6574 7272     handler : interr
  a3289c:	7075 2074 6469 5b78 6425 0a5d 0000 0000     upt idx[%d].....
  a328ac:	4f52 7c4d 6f4f 7370 2d20 6820 7261 2064     ROM|Oops - hard 
  a328bc:	6166 6c75 0d74 000a 4f52 7c4d 7865 6563     fault...ROM|exce
  a328cc:	7470 6f69 3a6e 7825 0a0d 0000 4f52 7c4d     ption:%x....ROM|
  a328dc:	6f4f 7370 4e3a 494d 000a 0000 4f52 7c4d     Oops:NMI....ROM|
  a328ec:	6f4f 7370 2d20 6920 736e 206e 6170 6567     Oops - insn page
  a328fc:	6620 7561 746c 0a0d 0000 0000 4f52 7c4d      fault......ROM|
  a3290c:	6f4f 7370 2d20 6c20 616f 2064 6170 6567     Oops - load page
  a3291c:	6620 7561 746c 0a0d 0000 0000 4f52 7c4d      fault......ROM|
  a3292c:	6f4f 7370 2d20 6c20 636f 206b 7075 0a0d     Oops - lock up..
  a3293c:	0000 0000 4f52 7c4d 6f4f 7370 2d20 7320     ....ROM|Oops - s
  a3294c:	6f74 6572 7020 6761 2065 6166 6c75 0d74     tore page fault.
  a3295c:	000a 0000 4f52 7c4d 6f4f 7370 2d20 6520     ....ROM|Oops - e
  a3296c:	7262 6165 0d6b 000a 4f52 7c4d 6f4f 7370     break...ROM|Oops
  a3297c:	2d20 6520 766e 7269 6e6f 656d 746e 6320      - environment c
  a3298c:	6c61 206c 7266 6d6f 4d20 6d2d 646f 0d65     all from M-mode.
  a3299c:	000a 0000 4f52 7c4d 6f4f 7370 2d20 6520     ....ROM|Oops - e
  a329ac:	766e 7269 6e6f 656d 746e 6320 6c61 206c     nvironment call 
  a329bc:	7266 6d6f 5320 6d2d 646f 0d65 000a 0000     from S-mode.....
  a329cc:	4f52 7c4d 6f4f 7370 2d20 6520 766e 7269     ROM|Oops - envir
  a329dc:	6e6f 656d 746e 6320 6c61 206c 7266 6d6f     onment call from
  a329ec:	5520 6d2d 646f 0d65 000a 0000 4f52 7c4d      U-mode.....ROM|
  a329fc:	6f4f 7370 2d20 6920 736e 7274 6375 6974     Oops - instructi
  a32a0c:	6e6f 6120 6363 7365 2073 6166 6c75 0d74     on access fault.
  a32a1c:	000a 0000 4f52 7c4d 6f4f 7370 2d20 6920     ....ROM|Oops - i
  a32a2c:	6c6c 6765 6c61 6920 736e 7274 6375 6974     llegal instructi
  a32a3c:	6e6f 0a0d 0000 0000 4f52 7c4d 6f4f 7370     on......ROM|Oops
  a32a4c:	2d20 6920 736e 7274 6375 6974 6e6f 6120      - instruction a
  a32a5c:	6464 6572 7373 6d20 7369 6c61 6769 656e     ddress misaligne
  a32a6c:	0d64 000a 4f52 7c4d 6f4f 7370 2d20 6c20     d...ROM|Oops - l
  a32a7c:	616f 2064 6361 6563 7373 6620 7561 746c     oad access fault
  a32a8c:	0a0d 0000 4f52 7c4d 6f4f 7370 2d20 6c20     ....ROM|Oops - l
  a32a9c:	616f 2064 6461 7264 7365 2073 696d 6173     oad address misa
  a32aac:	696c 6e67 6465 0a0d 0000 0000 4f52 7c4d     ligned......ROM|
  a32abc:	6f4f 7370 2d20 7320 6f74 6572 2820 726f     Oops - store (or
  a32acc:	4120 4f4d 2029 6361 6563 7373 6620 7561      AMO) access fau
  a32adc:	746c 0a0d 0000 0000 4f52 7c4d 6f4f 7370     lt......ROM|Oops
  a32aec:	2d20 7320 6f74 6572 2820 726f 4120 4f4d      - store (or AMO
  a32afc:	2029 6461 7264 7365 2073 696d 6173 696c     ) address misali
  a32b0c:	6e67 6465 0a0d 0000 4f52 7c4d 6f4f 7370     gned....ROM|Oops
  a32b1c:	2d20 7520 6b6e 6f6e 6e77 6520 6378 7065      - unknown excep
  a32b2c:	6974 6e6f 0a0d 0000 4f52 7c4d 2a2a 2a2a     tion....ROM|****
  a32b3c:	2a2a 2a2a 2a2a 2a2a 2a2a 7845 6563 7470     **********Except
  a32b4c:	6f69 206e 6e49 6f66 6d72 7461 6f69 2a6e     ion Information*
  a32b5c:	2a2a 2a2a 2a2a 2a2a 2a2a 2a2a 202a 000a     ************* ..
  a32b6c:	4f52 7c4d 7775 7845 5463 7079 2065 203d     ROM|uwExcType = 
  a32b7c:	7830 7825 000a 0000 4f52 7c4d 656d 6370     0x%x....ROM|mepc
  a32b8c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32b9c:	4f52 7c4d 736d 6174 7574 2073 2020 3d20     ROM|mstatus    =
  a32bac:	3020 2578 0a78 0000 4f52 7c4d 746d 6176      0x%x...ROM|mtva
  a32bbc:	206c 2020 2020 3d20 3020 2578 0a78 0000     l      = 0x%x...
  a32bcc:	4f52 7c4d 636d 7561 6573 2020 2020 3d20     ROM|mcause     =
  a32bdc:	3020 2578 0a78 0000 4f52 7c4d 6363 7561      0x%x...ROM|ccau
  a32bec:	6573 2020 2020 3d20 3020 2578 0a78 0000     se     = 0x%x...
  a32bfc:	4f52 7c4d 6172 2020 2020 2020 2020 3d20     ROM|ra         =
  a32c0c:	3020 2578 0a78 0000 4f52 7c4d 7073 2020      0x%x...ROM|sp  
  a32c1c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32c2c:	4f52 7c4d 7067 2020 2020 2020 2020 3d20     ROM|gp         =
  a32c3c:	3020 2578 0a78 0000 4f52 7c4d 7074 2020      0x%x...ROM|tp  
  a32c4c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32c5c:	4f52 7c4d 3074 2020 2020 2020 2020 3d20     ROM|t0         =
  a32c6c:	3020 2578 0a78 0000 4f52 7c4d 3174 2020      0x%x...ROM|t1  
  a32c7c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32c8c:	4f52 7c4d 3274 2020 2020 2020 2020 3d20     ROM|t2         =
  a32c9c:	3020 2578 0a78 0000 4f52 7c4d 3073 2020      0x%x...ROM|s0  
  a32cac:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32cbc:	4f52 7c4d 3173 2020 2020 2020 2020 3d20     ROM|s1         =
  a32ccc:	3020 2578 0a78 0000 4f52 7c4d 3061 2020      0x%x...ROM|a0  
  a32cdc:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32cec:	4f52 7c4d 3161 2020 2020 2020 2020 3d20     ROM|a1         =
  a32cfc:	3020 2578 0a78 0000 4f52 7c4d 3261 2020      0x%x...ROM|a2  
  a32d0c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32d1c:	4f52 7c4d 3361 2020 2020 2020 2020 3d20     ROM|a3         =
  a32d2c:	3020 2578 0a78 0000 4f52 7c4d 3461 2020      0x%x...ROM|a4  
  a32d3c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32d4c:	4f52 7c4d 3561 2020 2020 2020 2020 3d20     ROM|a5         =
  a32d5c:	3020 2578 0a78 0000 4f52 7c4d 3661 2020      0x%x...ROM|a6  
  a32d6c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32d7c:	4f52 7c4d 3761 2020 2020 2020 2020 3d20     ROM|a7         =
  a32d8c:	3020 2578 0a78 0000 4f52 7c4d 3273 2020      0x%x...ROM|s2  
  a32d9c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32dac:	4f52 7c4d 3373 2020 2020 2020 2020 3d20     ROM|s3         =
  a32dbc:	3020 2578 0a78 0000 4f52 7c4d 3473 2020      0x%x...ROM|s4  
  a32dcc:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32ddc:	4f52 7c4d 3573 2020 2020 2020 2020 3d20     ROM|s5         =
  a32dec:	3020 2578 0a78 0000 4f52 7c4d 3673 2020      0x%x...ROM|s6  
  a32dfc:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32e0c:	4f52 7c4d 3773 2020 2020 2020 2020 3d20     ROM|s7         =
  a32e1c:	3020 2578 0a78 0000 4f52 7c4d 3873 2020      0x%x...ROM|s8  
  a32e2c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32e3c:	4f52 7c4d 3973 2020 2020 2020 2020 3d20     ROM|s9         =
  a32e4c:	3020 2578 0a78 0000 4f52 7c4d 3173 2030      0x%x...ROM|s10 
  a32e5c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32e6c:	4f52 7c4d 3173 2031 2020 2020 2020 3d20     ROM|s11        =
  a32e7c:	3020 2578 0a78 0000 4f52 7c4d 3374 2020      0x%x...ROM|t3  
  a32e8c:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32e9c:	4f52 7c4d 3474 2020 2020 2020 2020 3d20     ROM|t4         =
  a32eac:	3020 2578 0a78 0000 4f52 7c4d 3574 2020      0x%x...ROM|t5  
  a32ebc:	2020 2020 2020 3d20 3020 2578 0a78 0000            = 0x%x...
  a32ecc:	4f52 7c4d 3674 2020 2020 2020 2020 3d20     ROM|t6         =
  a32edc:	3020 2578 0a78 0000 4f52 7c4d 2a2a 2a2a      0x%x...ROM|****
  a32eec:	2a2a 2a2a 2a2a 2a2a 2a2a 7845 6563 7470     **********Except
  a32efc:	6f69 206e 6e49 6f66 6d72 7461 6f69 206e     ion Information 
  a32f0c:	6e65 2a64 2a2a 2a2a 2a2a 2a2a 2a2a 2a2a     end*************
  a32f1c:	202a 000a 4f52 7c4d 6d6e 5f69 6168 646e     * ..ROM|nmi_hand
  a32f2c:	656c 2072 203a 6e69 6574 7272 7075 2074     ler : interrupt 
  a32f3c:	6469 5b78 6425 0a5d 0000 0000 555b 4750     idx[%d].....[UPG
  a32f4c:	205d 6567 2074 736d 6469 6620 6961 656c     ] get msid faile
  a32f5c:	2e64 7220 7465 3d20 3020 2578 0d78 000a     d. ret = 0x%x...
  a32f6c:	555b 4750 205d 7075 2067 6576 6972 7966     [UPG] upg verify
  a32f7c:	203a 736d 6469 6920 2073 7277 6e6f 2167     : msid is wrong!
  a32f8c:	0a0d 0000 555b 4750 205d 7075 7267 6461     ....[UPG] upgrad
  a32f9c:	2065 7270 676f 6572 7373 6320 6c61 626c     e progress callb
  a32fac:	6361 206b 6572 6967 7473 6620 6961 656c     ack regist faile
  a32fbc:	0d64 000a 555b 4750 205d 7075 2067 6570     d...[UPG] upg pe
  a32fcc:	6372 6e65 2074 203a 0000 0000 0031 0000     rcent : ....1...
  a32fdc:	2520 000d 555b 4750 205d 7075 7267 6461      %..[UPG] upgrad
  a32fec:	2065 6e69 7469 6620 6961 656c 2164 0a0d     e init failed!..
  a32ffc:	0000 0000 555b 4750 205d 7075 7267 6461     ....[UPG] upgrad
  a3300c:	2065 6e69 7469 4f20 214b 0a0d 0000 0000     e init OK!......
  a3301c:	555b 4750 205d 7075 5f67 6e69 7469 635f     [UPG] upg_init_c
  a3302c:	7972 6f74 635f 6766 6d20 6d65 7063 5f79     ryto_cfg memcpy_
  a3303c:	2073 6e65 7263 2079 6173 746c 6620 6961     s encry salt fai
  a3304c:	2e6c 0d20 000a 0000 555b 4750 205d 7075     l. .....[UPG] up
  a3305c:	5f67 6e69 7469 635f 7972 6f74 635f 6766     g_init_cryto_cfg
  a3306c:	6d20 6d65 7063 5f79 2073 6564 7263 2079      memcpy_s decry 
  a3307c:	6173 746c 6620 6961 2e6c 0d20 000a 0000     salt fail. .....
  a3308c:	555b 4750 205d 7075 5f67 6e69 7469 635f     [UPG] upg_init_c
  a3309c:	7972 6f74 635f 6766 6d20 6d65 7063 5f79     ryto_cfg memcpy_
  a330ac:	2073 6e65 7263 2079 7669 6620 6961 2e6c     s encry iv fail.
  a330bc:	0d20 000a 555b 4750 205d 7075 5f67 6e69      ...[UPG] upg_in
  a330cc:	7469 635f 7972 6f74 635f 6766 6d20 6d65     it_cryto_cfg mem
  a330dc:	7063 5f79 2073 6564 7263 2079 7669 6620     cpy_s decry iv f
  a330ec:	6961 2e6c 0d20 000a 555b 4750 205d 7264     ail. ...[UPG] dr
  a330fc:	5f76 6f72 5f6d 6963 6870 7265 735f 6d79     v_rom_cipher_sym
  a3310c:	5f63 6e69 7469 6620 6961 2c6c 7220 7465     c_init fail, ret
  a3311c:	3d20 3020 2578 2e78 0d20 000a 555b 4750      = 0x%x. ...[UPG
  a3312c:	205d 7264 5f76 6f72 5f6d 6963 6870 7265     ] drv_rom_cipher
  a3313c:	635f 6572 7461 5f65 656b 7379 6f6c 2074     _create_keyslot 
  a3314c:	6e65 7263 2079 6166 6c69 6465 202c 6572     encry failed, re
  a3315c:	2074 203d 7830 7825 202e 0a0d 0000 0000     t = 0x%x. ......
  a3316c:	555b 4750 205d 7264 5f76 6f72 5f6d 6963     [UPG] drv_rom_ci
  a3317c:	6870 7265 635f 6572 7461 5f65 656b 7379     pher_create_keys
  a3318c:	6f6c 2074 6564 7263 2079 6166 6c69 6465     lot decry failed
  a3319c:	202c 6572 2074 203d 7830 7825 202e 0a0d     , ret = 0x%x. ..
  a331ac:	0000 0000 555b 4750 205d 7075 5f67 6564     ....[UPG] upg_de
  a331bc:	7263 5f79 6f66 6174 705f 746b 643a 7672     cry_fota_pkt:drv
  a331cc:	725f 6d6f 635f 7069 6568 5f72 7973 636d     _rom_cipher_symc
  a331dc:	645f 6365 7972 7470 6620 6961 656c 2c64     _decrypt failed,
  a331ec:	7220 7465 3d20 3020 2578 2078 0a0d 0000      ret = 0x%x ....
  a331fc:	555b 4750 205d 7075 5f67 6e65 7263 5f79     [UPG] upg_encry_
  a3320c:	6f66 6174 705f 746b 643a 7672 725f 6d6f     fota_pkt:drv_rom
  a3321c:	635f 7069 6568 5f72 7973 636d 645f 6365     _cipher_symc_dec
  a3322c:	7972 7470 6620 6961 656c 2c64 7220 7465     rypt failed, ret
  a3323c:	3d20 3020 2578 2078 0a0d 0000 4f52 7c4d      = 0x%x ....ROM|
  a3324c:	705b 6e61 6369 695d 3a64 6425 632c 646f     [panic]id:%d,cod
  a3325c:	3a65 7830 7825 632c 6c61 3a6c 7830 7825     e:0x%x,call:0x%x
  a3326c:	0000 0000 555b 4750 205d 7075 5f67 6567     ....[UPG] upg_ge
  a3327c:	5f74 7075 7267 6461 5f65 6c66 6761 665f     t_upgrade_flag_f
  a3328c:	616c 6873 735f 6174 7472 615f 6464 2072     lash_start_addr 
  a3329c:	6166 6c69 0a0d 0000 555b 4750 205d 7075     fail....[UPG] up
  a332ac:	5f67 6c61 6f6c 5f63 6e61 5f64 6567 5f74     g_alloc_and_get_
  a332bc:	7075 7267 6461 5f65 6c66 6761 7520 6770     upgrade_flag upg
  a332cc:	6d5f 6c61 6f6c 2063 6166 6c69 0a0d 0000     _malloc fail....
  a332dc:	555b 4750 205d 7075 5f67 6c61 6f6c 5f63     [UPG] upg_alloc_
  a332ec:	6e61 5f64 6567 5f74 7075 7267 6461 5f65     and_get_upgrade_
  a332fc:	6c66 6761 7220 6165 2064 6c66 7361 2068     flag read flash 
  a3330c:	6166 6c69 0a0d 0000 555b 4750 205d 7075     fail....[UPG] up
  a3331c:	5f67 6c66 7361 5f68 6572 6461 6620 616c     g_flash_read fla
  a3332c:	2067 6166 6c69 0a0d 0000 0000 555b 4750     g fail......[UPG
  a3333c:	205d 7277 7469 2065 6f63 706d 656c 6574     ] write complete
  a3334c:	203a 7830 7825 0a0d 0000 0000 555b 4750     : 0x%x......[UPG
  a3335c:	205d 7075 5f67 6c66 7361 5f68 7277 7469     ] upg_flash_writ
  a3336c:	2065 7075 7267 6461 7265 6620 616c 2067     e upgrader flag 
  a3337c:	6166 6c69 202e 6572 2074 203d 7830 7825     fail. ret = 0x%x
  a3338c:	0a0d 0000 555b 4750 205d 6572 7274 2079     ....[UPG] retry 
  a3339c:	6974 656d 2073 6c61 206c 6166 6c69 6465     times all failed
  a333ac:	0a0d 0000 555b 4750 205d 6168 6873 6f5f     ....[UPG] hash_o
  a333bc:	6570 6172 6974 6e6f 203a 7264 5f76 6f72     peration: drv_ro
  a333cc:	5f6d 6963 6870 7265 685f 7361 2068 6166     m_cipher_hash fa
  a333dc:	6c69 7220 7465 765f 6c61 3d20 3020 2578     il ret_val = 0x%
  a333ec:	0d78 000a 555b 4750 205d 6163 636c 685f     x...[UPG] calc_h
  a333fc:	7361 2068 6166 6c69 7220 7465 3d20 3020     ash fail ret = 0
  a3340c:	2578 0d78 000a 0000 555b 4750 205d 6576     x%x.....[UPG] ve
  a3341c:	6972 7966 735f 6769 616e 7574 6572 6920     rify_signature i
  a3342c:	2073 6166 6c69 202c 6572 2074 203d 7830     s fail, ret = 0x
  a3343c:	7825 0a0d 0000 0000 555b 4750 205d 7075     %x......[UPG] up
  a3344c:	2067 6576 6972 7966 6820 6165 2064 4b4f     g verify head OK
  a3345c:	0a0d 0000 555b 4750 205d 7075 5f67 6567     ....[UPG] upg_ge
  a3346c:	5f74 6b70 5f67 6d69 6761 5f65 6168 6873     t_pkg_image_hash
  a3347c:	745f 6261 656c 6620 6961 0d6c 000a 0000     _table fail.....
  a3348c:	555b 4750 205d 7075 5f67 6576 6972 7966     [UPG] upg_verify
  a3349c:	685f 7361 2068 6d69 6761 2065 6174 6c62     _hash image tabl
  a334ac:	2065 4b4f 0a0d 0000 555b 4750 205d 7075     e OK....[UPG] up
  a334bc:	5f67 6567 5f74 6b70 5f67 6d69 6761 5f65     g_get_pkg_image_
  a334cc:	6568 6461 7265 6620 6961 0d6c 000a 0000     header fail.....
  a334dc:	555b 4750 205d 7075 2067 6576 6972 7966     [UPG] upg verify
  a334ec:	203a 6d69 6761 2065 4449 6520 7272 726f     : image ID error
  a334fc:	0a0d 0000 555b 4750 205d 6576 6972 7966     ....[UPG] verify
  a3350c:	665f 746f 5f61 656b 5f79 7261 6165 2d20     _fota_key_area -
  a3351c:	203e 6576 6972 7966 5320 4148 3532 0d36     > verify SHA256.
  a3352c:	000a 0000 555b 4750 205d 7075 2067 6576     ....[UPG] upg ve
  a3353c:	6972 7966 203a 656b 2079 7261 6165 6520     rify: key area e
  a3354c:	7272 726f 202e 6572 2074 203d 7830 7825     rror. ret = 0x%x
  a3355c:	0a0d 0000 555b 4750 205d 7075 2067 6576     ....[UPG] upg ve
  a3356c:	6972 7966 203a 6f66 6174 6920 666e 206f     rify: fota info 
  a3357c:	7265 6f72 2e72 7220 7465 3d20 3020 2578     error. ret = 0x%
  a3358c:	0d78 000a 555b 4750 205d 7075 2067 6576     x...[UPG] upg ve
  a3359c:	6972 7966 203a 7075 5f67 6863 6365 5f6b     rify: upg_check_
  a335ac:	6f66 6174 695f 666e 726f 616d 6974 6e6f     fota_information
  a335bc:	202e 6572 2074 203d 7830 7825 0a0d 0000     . ret = 0x%x....
  a335cc:	555b 4750 205d 7075 2067 6576 6972 7966     [UPG] upg verify
  a335dc:	203a 6966 656c 6920 616d 6567 6320 6568     : file image che
  a335ec:	6b63 6520 7272 726f 202e 6568 6461 7265     ck error. header
  a335fc:	6d5f 6761 6369 3d20 3020 2578 0d78 000a     _magic = 0x%x...
  a3360c:	555b 4750 205d 7075 2067 6576 6972 7966     [UPG] upg verify
  a3361c:	203a 6d69 6761 2065 6863 6365 206b 4b4f     : image check OK
  a3362c:	202e 6d69 6761 5f65 6469 3d20 3020 2578     . image_id = 0x%
  a3363c:	0d78 000a 555b 4750 205d 7075 5f67 6576     x...[UPG] upg_ve
  a3364c:	6972 7966 685f 7361 3a68 6820 7361 5f68     rify_hash: hash_
  a3365c:	706f 7265 7461 6f69 206e 6166 6c69 7220     operation fail r
  a3366c:	7465 765f 6c61 3d20 3020 2578 0d78 000a     et_val = 0x%x...
  a3367c:	555b 4750 205d 6576 6972 7966 665f 746f     [UPG] verify_fot
  a3368c:	5f61 6e69 6f66 2d20 203e 6576 6972 7966     a_info -> verify
  a3369c:	5320 4148 3532 0d36 000a 0000 555b 4750      SHA256.....[UPG
  a336ac:	205d 6576 6972 7966 665f 746f 5f61 6e69     ] verify_fota_in
  a336bc:	6f66 2d20 203e 6576 6972 7966 7320 6769     fo -> verify sig
  a336cc:	656e 0d64 000a 0000 555b 4750 205d 7075     ned.....[UPG] up
  a336dc:	5f67 6576 6972 7966 685f 7361 3a68 6d20     g_verify_hash: m
  a336ec:	6d65 6d63 2070 6168 6873 6620 6961 2e6c     emcmp hash fail.
  a336fc:	0a0d 0000 555b 4750 205d 7075 5f67 6567     ....[UPG] upg_ge
  a3370c:	5f74 6170 6b63 6761 5f65 6568 6461 7265     t_package_header
  a3371c:	6620 6961 0d6c 000a 555b 4750 205d 6175      fail...[UPG] ua
  a3372c:	6970 755f 6770 765f 7265 6669 5f79 6966     pi_upg_verify_fi
  a3373c:	656c 6620 6961 2c6c 7220 7465 3d20 3020     le fail, ret = 0
  a3374c:	2578 0d78 000a 0000 555b 4750 205d 7075     x%x.....[UPG] up
  a3375c:	5f67 6567 5f74 6966 6d72 6177 6572 6e5f     g_get_firmware_n
  a3376c:	6d75 6562 5f72 6e69 705f 6361 616b 6567     umber_in_package
  a3377c:	6620 6961 2c6c 7220 7465 3d20 3020 2578      fail, ret = 0x%
  a3378c:	0d78 000a 555b 4750 205d 7075 5f67 7075     x...[UPG] upg_up
  a3379c:	7267 6461 5f65 6572 7571 7365 2074 6166     grade_request fa
  a337ac:	6c69 202e 6572 2074 203d 7830 7825 0a0d     il. ret = 0x%x..
  a337bc:	0000 0000 555b 4750 205d 7075 5f67 6e65     ....[UPG] upg_en
  a337cc:	7263 5f79 6f66 6174 705f 746b 6d3a 6d65     cry_fota_pkt:mem
  a337dc:	6f6d 6576 735f 6620 6961 656c 2e64 0a0d     move_s failed...
  a337ec:	0000 0000 555b 4750 205d 7075 5f67 7a6c     ....[UPG] upg_lz
  a337fc:	616d 615f 6c6c 636f 6620 6961 656c 2164     ma_alloc failed!
  a3380c:	2121 7320 7a69 2065 203d 7830 7825 0a0d     !! size = 0x%x..
  a3381c:	0000 0000 555b 4750 205d 7075 5f67 7a6c     ....[UPG] upg_lz
  a3382c:	616d 725f 6165 5f64 6b70 5f74 6564 7263     ma_read_pkt_decr
  a3383c:	2079 6166 6c69 7220 7465 3d20 3020 2578     y fail ret = 0x%
  a3384c:	2e78 0a0d 0000 0000 555b 4750 205d 7075     x.......[UPG] up
  a3385c:	5f67 7a6c 616d 645f 6365 646f 5f65 6f74     g_lzma_decode_to
  a3386c:	625f 6675 6620 6961 206c 6572 2074 203d     _buf fail ret = 
  a3387c:	7830 7825 0d2e 000a 555b 4750 205d 7a6c     0x%x....[UPG] lz
  a3388c:	616d 6564 5f63 6564 6f63 6564 6f74 7562     madec_decodetobu
  a3389c:	2066 6166 6c69 7220 7465 3d20 3020 2578     f fail ret = 0x%
  a338ac:	2e78 0d20 000a 0000 555b 4750 205d 7075     x. .....[UPG] up
  a338bc:	5f67 7a6c 616d 775f 6972 6574 695f 616d     g_lzma_write_ima
  a338cc:	6567 6620 6961 2e6c 0a0d 0000 555b 4750     ge fail.....[UPG
  a338dc:	205d 7075 5f67 6e65 7263 5f79 6f66 6174     ] upg_encry_fota
  a338ec:	705f 746b 6620 6961 206c 6572 2074 203d     _pkt fail ret = 
  a338fc:	7830 7825 0d20 000a 3044 8000 555b 4750     0x%x ...D0..[UPG
  a3390c:	205d 6f4e 2074 656e 6465 7420 206f 7075     ] Not need to up
  a3391c:	7267 6461 2e65 2e2e 0a0d 0000 555b 4750     grade.......[UPG
  a3392c:	205d 6170 6b63 6761 2065 6e69 6f66 7420     ] package info t
  a3393c:	746f 6c61 203d 7830 7825 202c 6966 696e     otal= 0x%x, fini
  a3394c:	6873 6465 3d20 3020 2578 0d78 000a 0000     shed = 0x%x.....
  a3395c:	555b 4750 205d 7075 6164 6574 6920 616d     [UPG] update ima
  a3396c:	6567 6e20 6d75 6562 2072 203d 7830 7825     ge number = 0x%x
  a3397c:	0a0d 0000 555b 4750 205d 7075 6164 6574     ....[UPG] update
  a3398c:	6620 7269 776d 7261 2065 756e 626d 7265      firmware number
  a3399c:	3d20 3020 2578 0d78 000a 0000 555b 4750      = 0x%x.....[UPG
  a339ac:	205d 6854 2065 6d69 6761 2065 6168 2073     ] The image has 
  a339bc:	6966 696e 6873 6465 202e 6d69 6761 5f65     finished. image_
  a339cc:	6469 3d20 3020 2578 0d78 000a 555b 4750     id = 0x%x...[UPG
  a339dc:	205d 7075 5f67 6567 5f74 6b70 5f67 6d69     ] upg_get_pkg_im
  a339ec:	6761 5f65 6568 6461 7265 6620 6961 2e6c     age_header fail.
  a339fc:	0a0d 0000 555b 4750 205d 7075 5f67 6573     ....[UPG] upg_se
  a33a0c:	5f74 6966 6d72 6177 6572 755f 6470 7461     t_firmware_updat
  a33a1c:	5f65 7473 7461 7375 6620 6961 0d6c 000a     e_status fail...
  a33a2c:	555b 4750 205d 7473 7261 2074 6570 6672     [UPG] start perf
  a33a3c:	726f 206d 7075 6164 6574 6920 616d 6567     orm update image
  a33a4c:	3a20 3020 2578 0d78 000a 0000 555b 4750      : 0x%x.....[UPG
  a33a5c:	205d 7075 5f67 6c66 7361 5f68 7265 7361     ] upg_flash_eras
  a33a6c:	5f65 656d 6174 6164 6174 705f 6761 7365     e_metadata_pages
  a33a7c:	6620 6961 2e6c 0a0d 0000 0000 555b 4750      fail.......[UPG
  a33a8c:	205d 6d69 6761 2065 6564 6f63 706d 6572     ] image decompre
  a33a9c:	7373 665f 616c 3a67 3020 2578 0d78 000a     ss_flag: 0x%x...
  a33aac:	555b 4750 205d 6564 6f63 706d 6572 7373     [UPG] decompress
  a33abc:	7520 6770 0a0d 0000 555b 4750 205d 6964      upg....[UPG] di
  a33acc:	6666 7520 6770 0a0d 0000 0000 555b 4750     ff upg......[UPG
  a33adc:	205d 7566 6c6c 7520 6770 0a0d 0000 0000     ] full upg......
  a33aec:	555b 4750 205d 7773 7469 6863 7320 6174     [UPG] switch sta
  a33afc:	7574 2073 203d 7830 7825 0a0d 0000 0000     tus = 0x%x......
  a33b0c:	555b 4750 205d 6570 6672 726f 206d 7075     [UPG] perform up
  a33b1c:	6164 6574 6920 616d 6567 6f20 6576 2e72     date image over.
  a33b2c:	7220 7465 3d20 3020 2578 0d78 000a 0000      ret = 0x%x.....
  a33b3c:	555b 4750 205d 7473 7261 2074 6570 6672     [UPG] start perf
  a33b4c:	726f 206d 564e 6920 616d 6567 3a20 3020     orm NV image : 0
  a33b5c:	2578 0d78 000a 0000 555b 4750 205d 6570     x%x.....[UPG] pe
  a33b6c:	6672 726f 206d 564e 6920 616d 6567 6f20     rform NV image o
  a33b7c:	6576 2e72 7220 7465 3d20 3020 2578 0d78     ver. ret = 0x%x.
  a33b8c:	000a 0000 555b 4750 205d 7075 5f67 7270     ....[UPG] upg_pr
  a33b9c:	636f 7365 5f73 7075 6164 6574 695f 616d     ocess_update_ima
  a33bac:	6567 745f 7361 736b 6620 6961 2c6c 7220     ge_tasks fail, r
  a33bbc:	7465 3d20 3020 2578 0d78 000a 555b 4750     et = 0x%x...[UPG
  a33bcc:	205d 7061 2070 6973 657a 6920 2073 6f6e     ] app size is no
  a33bdc:	2074 6e65 756f 6867 2021 7061 5f70 6973     t enough! app_si
  a33bec:	657a 3d20 3020 2578 2c78 6420 6365 6d6f     ze = 0x%x, decom
  a33bfc:	7270 7365 5f73 656c 206e 203d 7830 7825     press_len = 0x%x
  a33c0c:	0a0d 0000 555b 4750 205d 7075 5f67 7a6c     ....[UPG] upg_lz
  a33c1c:	616d 645f 6365 646f 2065 6166 6c69 7220     ma_decode fail r
  a33c2c:	7465 3d20 3020 2578 0d78 000a 0ae2 00a3     et = 0x%x.......
  a33c3c:	0b10 00a3 0b24 00a3 0b6a 00a3 0bc8 00a3     ....$...j.......
  a33c4c:	0bcc 00a3 0c0a 00a3 0d22 00a3               ........"...

00a33c58 <g_itoaLowerDigits>:
  a33c58:	3130 3332 3534 3736 3938 6261 6463 6665     0123456789abcdef
  a33c68:	0078 0000                                   x...

00a33c6c <g_itoaUpperDigits>:
  a33c6c:	3130 3332 3534 3736 3938 4241 4443 4645     0123456789ABCDEF
  a33c7c:	0058 0000                                   X...

00a33c80 <stateTable.1749>:
	...
  a33ca0:	0006 0600 0100 0000 0000 0603 0600 0002     ................
  a33cb0:	0504 0505 0505 0505 0505 0000 0000 0000     ................
  a33cc0:	0000 0800 0800 0808 0700 0000 0007 0007     ................
  a33cd0:	0000 0800 0000 0000 0008 0007 0000 0000     ................
  a33ce0:	0000 0800 0808 0808 0807 0007 0007 0800     ................
  a33cf0:	0708 0800 0807 0700 0008 0007 0000 0000     ................
	...
  a33d80:	0800 0808 0808 0008 0100 0800 0808 0808     ................
  a33d90:	0001 0400 0404 0808 0008 0000 0303 0508     ................
  a33da0:	0808 0000 0200 0302 0505 0008 0000 0303     ................
  a33db0:	0503 0805 0000 0200 0202 0808 0008 0000     ................
  a33dc0:	0606 0606 0606 0000 0700 0707 0707 0007     ................
  a33dd0:	0000 0000                                   ....
