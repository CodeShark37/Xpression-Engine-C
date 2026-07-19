/**
 * @file validate.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-06-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
 
#ifndef VALIDATE_H
#define VALIDATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "xpression.h"

#define VALIDATE_MAX_DEPTH 256

bool validate_xpression(XpressionContext* xpr);

/* Map NodeKind to ArgKindMask - ensure NODE_FUNCTION is handled explicitly */
//ArgKindMask nodekind_to_mask(NodeKind k);

/* Map ArgKindMask to NodeKind  - ensure NODE_FUNCTION is handled explicitly */
//NodeKind mask_to_nodekind(ArgKindMask k);


#ifdef __cplusplus
}
#endif

#endif /* VALIDATE_H */
