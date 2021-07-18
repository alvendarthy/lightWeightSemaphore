#ifndef GCC_ATOM_H
#define GCC_ATOM_H

#define barrier()                 (__sync_synchronize())
#define AO_GET(ptr)               ({ __typeof__(*(ptr)) volatile *_val = (ptr); barrier(); (*_val); })
#define AO_SET(ptr, value)        ((void)__sync_lock_test_and_set((ptr), (value)))
#define AO_SWAP(ptr, value)       ((__typeof__(*(ptr)))__sync_lock_test_and_set((ptr), (value)))
#define AO_CAS(ptr, comp, value)  ((__typeof__(*(ptr)))__sync_val_compare_and_swap((ptr), (comp), (value)))
#define AO_CASB(ptr, comp, value) (__sync_bool_compare_and_swap((ptr), (comp), (value)) != 0 ? true : false)
#define AO_CLEAR(ptr)             ((void)__sync_lock_release((ptr)))
#define AO_ADD_F(ptr, value)      ((__typeof__(*(ptr)))__sync_add_and_fetch((ptr), (value)))
#define AO_SUB_F(ptr, value)      ((__typeof__(*(ptr)))__sync_sub_and_fetch((ptr), (value)))
#define AO_OR_F(ptr, value)       ((__typeof__(*(ptr)))__sync_or_and_fetch((ptr), (value)))
#define AO_AND_F(ptr, value)      ((__typeof__(*(ptr)))__sync_and_and_fetch((ptr), (value)))
#define AO_XOR_F(ptr, value)      ((__typeof__(*(ptr)))__sync_xor_and_fetch((ptr), (value)))
#define AO_F_ADD(ptr, value)      ((__typeof__(*(ptr)))__sync_fetch_and_add((ptr), (value)))
#define AO_F_SUB(ptr, value)      ((__typeof__(*(ptr)))__sync_fetch_and_sub((ptr), (value)))
#define AO_F_OR(ptr, value)       ((__typeof__(*(ptr)))__sync_fetch_and_or((ptr), (value)))
#define AO_F_AND(ptr, value)      ((__typeof__(*(ptr)))__sync_fetch_and_and((ptr), (value)))
#define AO_F_XOR(ptr, value)      ((__typeof__(*(ptr)))__sync_fetch_and_xor((ptr), (value)))

#define AO_INC(ptr)                 ((void)AO_ADD_F((ptr), 1))
#define AO_DEC(ptr)                 ((void)AO_SUB_F((ptr), 1))
#define AO_ADD(ptr, val)            ((void)AO_ADD_F((ptr), (val)))
#define AO_SUB(ptr, val)            ((void)AO_SUB_F((ptr), (val)))
#define AO_OR(ptr, val)          ((void)AO_OR_F((ptr), (val)))
#define AO_AND(ptr, val)            ((void)AO_AND_F((ptr), (val)))
#define AO_XOR(ptr, val)            ((void)AO_XOR_F((ptr), (val)))
#define AO_BIT_ON(ptr, mask)        AO_OR_F((ptr), (mask))
#define AO_BIT_OFF(ptr, mask)       AO_AND_F((ptr), ~(mask))
#define AO_BIT_XCHG(ptr, mask)      AO_XOR_F((ptr), (mask))

#define CAS_bool(mem,oldV,newV)    __sync_bool_compare_and_swap((mem),(oldV),(newV))
#define CAS_fetch(mem,oldV,newV)   __sync_val_compare_and_swap((mem),(oldV),(newV)) 

#endif
