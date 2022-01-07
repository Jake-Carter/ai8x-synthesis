/*******************************************************************************
* Copyright (C) 2019-2022 Maxim Integrated Products, Inc., All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************/

// aisegment_unet
// Created using ai8xize.py --test-dir sdk/Examples/MAX78002/CNN --prefix aisegment_unet --checkpoint-file trained/ai85-aisegment-unet-large-fakept-q.pth.tar --config-file networks/aisegment-unet-large-fakept.yaml --overwrite --device MAX78002 --compact-data --mexpress --timer 0 --display-checkpoint --verbose --overlap-data --mlator --no-unload --max-checklines 8192

// DO NOT EDIT - regenerate this file instead!

// Configuring 19 layers
// Input data: HWC
// Layer 0: 48x48x48, no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, ReLU, 64x48x48 output
// Layer 1: 64x48x48, no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, ReLU, 64x48x48 output
// Layer 2: 64x48x48, no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, ReLU, 32x48x48 output
// Layer 3: 32x48x48, no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, ReLU, 8x48x48 output
// Layer 4: 8x48x48, max pool 2x2 with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, ReLU, 28x24x24 output
// Layer 5: 28x24x24, max pool 2x2 with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, ReLU, 56x12x12 output
// Layer 6: 56x12x12, max pool 2x2 with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, ReLU, 112x6x6 output
// Layer 7: 56x12x12, no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, no activation, 56x12x12 output
// Layer 8: 112x6x6, no pooling, convtranspose2d with kernel size 3x3, stride 2/2, pad 1/1, no activation, 56x12x12 output
// Layer 9: 112x12x12, no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, ReLU, 56x12x12 output
// Layer 10: 56x12x12, no pooling, convtranspose2d with kernel size 3x3, stride 2/2, pad 1/1, no activation, 28x24x24 output
// Layer 11: 56x24x24, no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, ReLU, 28x24x24 output
// Layer 12: 28x24x24, no pooling, convtranspose2d with kernel size 3x3, stride 2/2, pad 1/1, no activation, 8x48x48 output
// Layer 13: 16x48x48, no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, ReLU, 48x48x48 output
// Layer 14: 48x48x48, no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, ReLU, 64x48x48 output
// Layer 15: 64x48x48, no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, ReLU, 64x48x48 output
// Layer 16: 64x48x48, no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, ReLU, 64x48x48 output
// Layer 17: 64x48x48, no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, no activation, 64x48x48 output
// Layer 18: 64x48x48, no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, no activation, 32x48x48 output

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc.h"
#include "gcfr_regs.h"
#include "cnn.h"
#include "weights.h"

void CNN_ISR(void)
{
  // Acknowledge interrupt to all quadrants
  *((volatile uint32_t *) 0x51000000) &= ~((1<<12) | 1);
  *((volatile uint32_t *) 0x52000000) &= ~((1<<12) | 1);
  *((volatile uint32_t *) 0x53000000) &= ~((1<<12) | 1);
  *((volatile uint32_t *) 0x54000000) &= ~((1<<12) | 1);

  CNN_COMPLETE; // Signal that processing is complete
#ifdef CNN_INFERENCE_TIMER
  cnn_time = MXC_TMR_SW_Stop(CNN_INFERENCE_TIMER);
#else
  cnn_time = 1;
#endif
}

int cnn_continue(void)
{
  cnn_time = 0;

  *((volatile uint32_t *) 0x51000000) |= 1; // Re-enable quadrant 0

  return CNN_OK;
}

int cnn_stop(void)
{
  *((volatile uint32_t *) 0x51000000) &= ~1; // Disable quadrant 0

  return CNN_OK;
}

void memcpy32(uint32_t *dst, const uint32_t *src, int n)
{
  while (n-- > 0) {
    *dst++ = *src++;
  }
}

static const uint32_t kernels[] = KERNELS;

int cnn_load_weights(void)
{
  uint32_t len;
  volatile uint32_t *addr;
  const uint32_t *ptr = kernels;

  while ((addr = (volatile uint32_t *) *ptr++) != 0) {
    *((volatile uint8_t *) ((uint32_t) addr | 1)) = 0x01; // Set address
    len = *ptr++;
    while (len-- > 0)
      *addr++ = *ptr++;
  }

  return CNN_OK;
}

static const uint8_t bias_0[] = BIAS_0;
static const uint8_t bias_1[] = BIAS_1;
static const uint8_t bias_2[] = BIAS_2;
static const uint8_t bias_3[] = BIAS_3;

static void memcpy_8to32(uint32_t *dst, const uint8_t *src, int n)
{
  while (n-- > 0) {
    *dst++ = *src++;
  }
}

int cnn_load_bias(void)
{
  memcpy_8to32((uint32_t *) 0x51180000, bias_0, sizeof(uint8_t) * 228);
  memcpy_8to32((uint32_t *) 0x52180000, bias_1, sizeof(uint8_t) * 220);
  memcpy_8to32((uint32_t *) 0x53180000, bias_2, sizeof(uint8_t) * 212);
  memcpy_8to32((uint32_t *) 0x54180000, bias_3, sizeof(uint8_t) * 216);

  return CNN_OK;
}

int cnn_init(void)
{
  *((volatile uint32_t *) 0x51000000) = 0x00000008; // Enable clocks
  *((volatile uint32_t *) 0x52000000) = 0x00000008; // Enable clocks
  *((volatile uint32_t *) 0x53000000) = 0x00000008; // Enable clocks
  *((volatile uint32_t *) 0x54000000) = 0x00000008; // Enable clocks
  *((volatile uint32_t *) 0x50001000) = 0x00000000; // AON control
  *((volatile uint32_t *) 0x51000004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x52000004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x53000004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x54000004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x5100000c) = 0x00001c80; // Clear registers
  *((volatile uint32_t *) 0x5200000c) = 0x00001c80; // Clear registers
  *((volatile uint32_t *) 0x5300000c) = 0x00001c80; // Clear registers
  *((volatile uint32_t *) 0x5400000c) = 0x00001c80; // Clear registers
  while ((*((volatile uint32_t *) 0x5100000c) & 0x2000000) != 0x2000000); // Wait for clear
  while ((*((volatile uint32_t *) 0x5200000c) & 0x2000000) != 0x2000000); // Wait for clear
  while ((*((volatile uint32_t *) 0x5300000c) & 0x2000000) != 0x2000000); // Wait for clear
  while ((*((volatile uint32_t *) 0x5400000c) & 0x2000000) != 0x2000000); // Wait for clear
  *((volatile uint32_t *) 0x5100000c) = 0x00000000; // Reset BIST
  *((volatile uint32_t *) 0x5200000c) = 0x00000000; // Reset BIST
  *((volatile uint32_t *) 0x5300000c) = 0x00000000; // Reset BIST
  *((volatile uint32_t *) 0x5400000c) = 0x00000000; // Reset BIST

  *((volatile uint32_t *) 0x51000000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x51000008) = 0x00000012; // Layer count
  *((volatile uint32_t *) 0x52000000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x52000008) = 0x00000012; // Layer count
  *((volatile uint32_t *) 0x53000000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x53000008) = 0x00000012; // Layer count
  *((volatile uint32_t *) 0x54000000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x54000008) = 0x00000012; // Layer count

  return CNN_OK;
}

int cnn_configure(void)
{
  // Layer 0 quadrant 0
  *((volatile uint32_t *) 0x51100004) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x51100008) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x51100018) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5110001c) = 0x00000180; // SRAM write ptr
  *((volatile uint32_t *) 0x51100020) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x51100024) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110002c) = 0x000001c0; // SRAM read ptr
  *((volatile uint32_t *) 0x51100030) = 0x00006b20; // Layer control
  *((volatile uint32_t *) 0x51100034) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x51100038) = 0x000001f8; // Mask count
  *((volatile uint32_t *) 0x51100040) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5110000c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5110004c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x51100048) = 0xffffffff; // Mask and processor enables

  // Layer 0 quadrant 1
  *((volatile uint32_t *) 0x52100004) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x52100008) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x52100018) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5210001c) = 0x00000180; // SRAM write ptr
  *((volatile uint32_t *) 0x52100020) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x52100024) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210002c) = 0x000001c0; // SRAM read ptr
  *((volatile uint32_t *) 0x52100030) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x52100034) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x52100038) = 0x000001f8; // Mask count
  *((volatile uint32_t *) 0x52100040) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5210000c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5210004c) = 0x00003000; // Post processing register
  *((volatile uint32_t *) 0x52100048) = 0xffffffff; // Mask and processor enables

  // Layer 0 quadrant 2
  *((volatile uint32_t *) 0x53100004) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x53100008) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x53100018) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5310001c) = 0x00000180; // SRAM write ptr
  *((volatile uint32_t *) 0x53100020) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x53100024) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310002c) = 0x000001c0; // SRAM read ptr
  *((volatile uint32_t *) 0x53100030) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x53100034) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x53100038) = 0x000001f8; // Mask count
  *((volatile uint32_t *) 0x53100040) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5310000c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5310004c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x53100048) = 0xffffffff; // Mask and processor enables

  // Layer 0 quadrant 3
  *((volatile uint32_t *) 0x54100004) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x54100008) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x54100018) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5410001c) = 0x00000180; // SRAM write ptr
  *((volatile uint32_t *) 0x54100020) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x54100024) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410002c) = 0x000001c0; // SRAM read ptr
  *((volatile uint32_t *) 0x54100030) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x54100034) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x54100038) = 0x000001f8; // Mask count
  *((volatile uint32_t *) 0x54100040) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5410000c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5410004c) = 0x00002000; // Post processing register

  // Layer 1 quadrant 0
  *((volatile uint32_t *) 0x51100104) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x51100108) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x51100118) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5110011c) = 0x00000100; // SRAM write ptr
  *((volatile uint32_t *) 0x51100120) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x51100124) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110012c) = 0x00000180; // SRAM read ptr
  *((volatile uint32_t *) 0x51100130) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x51100134) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x51100138) = 0x00000438; // Mask count
  *((volatile uint32_t *) 0x5110013c) = 0x00000240; // Mask offset
  *((volatile uint32_t *) 0x51100140) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5110010c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x51100148) = 0xffffffff; // Mask and processor enables

  // Layer 1 quadrant 1
  *((volatile uint32_t *) 0x52100104) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x52100108) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x52100118) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5210011c) = 0x00000100; // SRAM write ptr
  *((volatile uint32_t *) 0x52100120) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x52100124) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210012c) = 0x00000180; // SRAM read ptr
  *((volatile uint32_t *) 0x52100130) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x52100134) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x52100138) = 0x00000438; // Mask count
  *((volatile uint32_t *) 0x5210013c) = 0x00000240; // Mask offset
  *((volatile uint32_t *) 0x52100140) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5210010c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x52100148) = 0xffffffff; // Mask and processor enables

  // Layer 1 quadrant 2
  *((volatile uint32_t *) 0x53100104) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x53100108) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x53100118) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5310011c) = 0x00000100; // SRAM write ptr
  *((volatile uint32_t *) 0x53100120) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x53100124) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310012c) = 0x00000180; // SRAM read ptr
  *((volatile uint32_t *) 0x53100130) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x53100134) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x53100138) = 0x00000438; // Mask count
  *((volatile uint32_t *) 0x5310013c) = 0x00000240; // Mask offset
  *((volatile uint32_t *) 0x53100140) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5310010c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5310014c) = 0x00001000; // Post processing register
  *((volatile uint32_t *) 0x53100148) = 0xffffffff; // Mask and processor enables

  // Layer 1 quadrant 3
  *((volatile uint32_t *) 0x54100104) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x54100108) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x54100118) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5410011c) = 0x00000100; // SRAM write ptr
  *((volatile uint32_t *) 0x54100120) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x54100124) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410012c) = 0x00000180; // SRAM read ptr
  *((volatile uint32_t *) 0x54100130) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x54100134) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x54100138) = 0x00000438; // Mask count
  *((volatile uint32_t *) 0x5410013c) = 0x00000240; // Mask offset
  *((volatile uint32_t *) 0x54100140) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5410010c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x54100148) = 0xffffffff; // Mask and processor enables

  // Layer 2 quadrant 0
  *((volatile uint32_t *) 0x51100204) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x51100208) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x51100218) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5110021c) = 0x00000080; // SRAM write ptr
  *((volatile uint32_t *) 0x51100220) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x51100224) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110022c) = 0x00000100; // SRAM read ptr
  *((volatile uint32_t *) 0x51100230) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x51100234) = 0x000f8000; // Layer control 2
  *((volatile uint32_t *) 0x51100238) = 0x00000578; // Mask count
  *((volatile uint32_t *) 0x5110023c) = 0x00000480; // Mask offset
  *((volatile uint32_t *) 0x51100240) = 0x0000001f; // Output channel count
  *((volatile uint32_t *) 0x5110020c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5110024c) = 0x000010a8; // Post processing register
  *((volatile uint32_t *) 0x51100248) = 0xffffffff; // Mask and processor enables

  // Layer 2 quadrant 1
  *((volatile uint32_t *) 0x52100204) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x52100208) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x52100218) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5210021c) = 0x00000080; // SRAM write ptr
  *((volatile uint32_t *) 0x52100220) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x52100224) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210022c) = 0x00000100; // SRAM read ptr
  *((volatile uint32_t *) 0x52100230) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x52100234) = 0x000f8000; // Layer control 2
  *((volatile uint32_t *) 0x52100238) = 0x00000578; // Mask count
  *((volatile uint32_t *) 0x5210023c) = 0x00000480; // Mask offset
  *((volatile uint32_t *) 0x52100240) = 0x0000001f; // Output channel count
  *((volatile uint32_t *) 0x5210020c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x52100248) = 0xffffffff; // Mask and processor enables

  // Layer 2 quadrant 2
  *((volatile uint32_t *) 0x53100204) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x53100208) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x53100218) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5310021c) = 0x00000080; // SRAM write ptr
  *((volatile uint32_t *) 0x53100220) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x53100224) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310022c) = 0x00000100; // SRAM read ptr
  *((volatile uint32_t *) 0x53100230) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x53100234) = 0x000f8000; // Layer control 2
  *((volatile uint32_t *) 0x53100238) = 0x00000578; // Mask count
  *((volatile uint32_t *) 0x5310023c) = 0x00000480; // Mask offset
  *((volatile uint32_t *) 0x53100240) = 0x0000001f; // Output channel count
  *((volatile uint32_t *) 0x5310020c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x53100248) = 0xffffffff; // Mask and processor enables

  // Layer 2 quadrant 3
  *((volatile uint32_t *) 0x54100204) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x54100208) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x54100218) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5410021c) = 0x00000080; // SRAM write ptr
  *((volatile uint32_t *) 0x54100220) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x54100224) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410022c) = 0x00000100; // SRAM read ptr
  *((volatile uint32_t *) 0x54100230) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x54100234) = 0x000f8000; // Layer control 2
  *((volatile uint32_t *) 0x54100238) = 0x00000578; // Mask count
  *((volatile uint32_t *) 0x5410023c) = 0x00000480; // Mask offset
  *((volatile uint32_t *) 0x54100240) = 0x0000001f; // Output channel count
  *((volatile uint32_t *) 0x5410020c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x54100248) = 0xffffffff; // Mask and processor enables

  // Layer 3 quadrant 0
  *((volatile uint32_t *) 0x51100304) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x51100308) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x51100318) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5110031c) = 0x00070000; // SRAM write ptr
  *((volatile uint32_t *) 0x51100324) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110032c) = 0x00000080; // SRAM read ptr
  *((volatile uint32_t *) 0x51100330) = 0x0088ab20; // Layer control
  *((volatile uint32_t *) 0x51100334) = 0x00038000; // Layer control 2
  *((volatile uint32_t *) 0x51100338) = 0x000000d8; // Mask count
  *((volatile uint32_t *) 0x5110033c) = 0x000000a0; // Mask offset
  *((volatile uint32_t *) 0x51100340) = 0x00000007; // Output channel count
  *((volatile uint32_t *) 0x51100344) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x5110034c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x51100348) = 0xffffffff; // Mask and processor enables

  // Layer 3 quadrant 1
  *((volatile uint32_t *) 0x52100304) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x52100308) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x52100318) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5210031c) = 0x00070000; // SRAM write ptr
  *((volatile uint32_t *) 0x52100324) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210032c) = 0x00000080; // SRAM read ptr
  *((volatile uint32_t *) 0x52100330) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x52100334) = 0x00038000; // Layer control 2
  *((volatile uint32_t *) 0x52100338) = 0x000000d8; // Mask count
  *((volatile uint32_t *) 0x5210033c) = 0x000000a0; // Mask offset
  *((volatile uint32_t *) 0x52100340) = 0x00000007; // Output channel count
  *((volatile uint32_t *) 0x52100344) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x5210034c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x52100348) = 0xffffffff; // Mask and processor enables

  // Layer 3 quadrant 2
  *((volatile uint32_t *) 0x53100304) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x53100308) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x53100318) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5310031c) = 0x00070000; // SRAM write ptr
  *((volatile uint32_t *) 0x53100324) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310032c) = 0x00000080; // SRAM read ptr
  *((volatile uint32_t *) 0x53100330) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x53100334) = 0x00038000; // Layer control 2
  *((volatile uint32_t *) 0x53100338) = 0x000000d8; // Mask count
  *((volatile uint32_t *) 0x5310033c) = 0x000000a0; // Mask offset
  *((volatile uint32_t *) 0x53100340) = 0x00000007; // Output channel count
  *((volatile uint32_t *) 0x53100344) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x5310034c) = 0x00022000; // Post processing register

  // Layer 3 quadrant 3
  *((volatile uint32_t *) 0x54100304) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x54100308) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x54100318) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5410031c) = 0x00070000; // SRAM write ptr
  *((volatile uint32_t *) 0x54100324) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410032c) = 0x00000080; // SRAM read ptr
  *((volatile uint32_t *) 0x54100330) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x54100334) = 0x00038000; // Layer control 2
  *((volatile uint32_t *) 0x54100338) = 0x000000d8; // Mask count
  *((volatile uint32_t *) 0x5410033c) = 0x000000a0; // Mask offset
  *((volatile uint32_t *) 0x54100340) = 0x00000007; // Output channel count
  *((volatile uint32_t *) 0x54100344) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x5410034c) = 0x000230d0; // Post processing register

  // Layer 4 quadrant 0
  *((volatile uint32_t *) 0x51100404) = 0x0032802e; // Rows
  *((volatile uint32_t *) 0x51100408) = 0x0002802e; // Columns
  *((volatile uint32_t *) 0x51100410) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x51100414) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x51100418) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5110041c) = 0x00038000; // SRAM write ptr
  *((volatile uint32_t *) 0x51100424) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100430) = 0x0088aba0; // Layer control
  *((volatile uint32_t *) 0x51100434) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x51100438) = 0x00000178; // Mask count
  *((volatile uint32_t *) 0x5110043c) = 0x000000a0; // Mask offset
  *((volatile uint32_t *) 0x51100440) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x51100444) = 0x00000017; // TRAM ptr max

  // Layer 4 quadrant 1
  *((volatile uint32_t *) 0x52100404) = 0x0032802e; // Rows
  *((volatile uint32_t *) 0x52100408) = 0x0002802e; // Columns
  *((volatile uint32_t *) 0x52100410) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x52100414) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x52100418) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5210041c) = 0x00038000; // SRAM write ptr
  *((volatile uint32_t *) 0x52100424) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100430) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x52100434) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x52100438) = 0x00000178; // Mask count
  *((volatile uint32_t *) 0x5210043c) = 0x000000a0; // Mask offset
  *((volatile uint32_t *) 0x52100440) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x52100444) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x5210044c) = 0x000010b8; // Post processing register

  // Layer 4 quadrant 2
  *((volatile uint32_t *) 0x53100404) = 0x0032802e; // Rows
  *((volatile uint32_t *) 0x53100408) = 0x0002802e; // Columns
  *((volatile uint32_t *) 0x53100410) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x53100414) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x53100418) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5310041c) = 0x00038000; // SRAM write ptr
  *((volatile uint32_t *) 0x53100424) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100430) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x53100434) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x53100438) = 0x00000178; // Mask count
  *((volatile uint32_t *) 0x5310043c) = 0x000000a0; // Mask offset
  *((volatile uint32_t *) 0x53100440) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x53100444) = 0x00000017; // TRAM ptr max

  // Layer 4 quadrant 3
  *((volatile uint32_t *) 0x54100404) = 0x0032802e; // Rows
  *((volatile uint32_t *) 0x54100408) = 0x0002802e; // Columns
  *((volatile uint32_t *) 0x54100410) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x54100414) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x54100418) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5410041c) = 0x00038000; // SRAM write ptr
  *((volatile uint32_t *) 0x54100424) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100430) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x54100434) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x54100438) = 0x00000178; // Mask count
  *((volatile uint32_t *) 0x5410043c) = 0x000000a0; // Mask offset
  *((volatile uint32_t *) 0x54100440) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x54100444) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x54100448) = 0xff00ff00; // Mask and processor enables

  // Layer 5 quadrant 0
  *((volatile uint32_t *) 0x51100504) = 0x001a8016; // Rows
  *((volatile uint32_t *) 0x51100508) = 0x00028016; // Columns
  *((volatile uint32_t *) 0x51100510) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x51100514) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x51100518) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5110051c) = 0x00001400; // SRAM write ptr
  *((volatile uint32_t *) 0x51100524) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100530) = 0x0088eba0; // Layer control
  *((volatile uint32_t *) 0x51100534) = 0x001b8000; // Layer control 2
  *((volatile uint32_t *) 0x51100538) = 0x00000298; // Mask count
  *((volatile uint32_t *) 0x5110053c) = 0x000000e0; // Mask offset
  *((volatile uint32_t *) 0x51100540) = 0x00000037; // Output channel count
  *((volatile uint32_t *) 0x51100544) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5110054c) = 0x00001070; // Post processing register

  // Layer 5 quadrant 1
  *((volatile uint32_t *) 0x52100504) = 0x001a8016; // Rows
  *((volatile uint32_t *) 0x52100508) = 0x00028016; // Columns
  *((volatile uint32_t *) 0x52100510) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x52100514) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x52100518) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5210051c) = 0x00001400; // SRAM write ptr
  *((volatile uint32_t *) 0x52100524) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100530) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x52100534) = 0x001b8000; // Layer control 2
  *((volatile uint32_t *) 0x52100538) = 0x00000298; // Mask count
  *((volatile uint32_t *) 0x5210053c) = 0x000000e0; // Mask offset
  *((volatile uint32_t *) 0x52100540) = 0x00000037; // Output channel count
  *((volatile uint32_t *) 0x52100544) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x52100548) = 0xf000f000; // Mask and processor enables

  // Layer 5 quadrant 2
  *((volatile uint32_t *) 0x53100504) = 0x001a8016; // Rows
  *((volatile uint32_t *) 0x53100508) = 0x00028016; // Columns
  *((volatile uint32_t *) 0x53100510) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x53100514) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x53100518) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5310051c) = 0x00001400; // SRAM write ptr
  *((volatile uint32_t *) 0x53100524) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100530) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x53100534) = 0x001b8000; // Layer control 2
  *((volatile uint32_t *) 0x53100538) = 0x00000298; // Mask count
  *((volatile uint32_t *) 0x5310053c) = 0x000000e0; // Mask offset
  *((volatile uint32_t *) 0x53100540) = 0x00000037; // Output channel count
  *((volatile uint32_t *) 0x53100544) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x53100548) = 0xffffffff; // Mask and processor enables

  // Layer 5 quadrant 3
  *((volatile uint32_t *) 0x54100504) = 0x001a8016; // Rows
  *((volatile uint32_t *) 0x54100508) = 0x00028016; // Columns
  *((volatile uint32_t *) 0x54100510) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x54100514) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x54100518) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5410051c) = 0x00001400; // SRAM write ptr
  *((volatile uint32_t *) 0x54100524) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100530) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x54100534) = 0x001b8000; // Layer control 2
  *((volatile uint32_t *) 0x54100538) = 0x00000298; // Mask count
  *((volatile uint32_t *) 0x5410053c) = 0x000000e0; // Mask offset
  *((volatile uint32_t *) 0x54100540) = 0x00000037; // Output channel count
  *((volatile uint32_t *) 0x54100544) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x54100548) = 0x00ff00ff; // Mask and processor enables

  // Layer 6 quadrant 0
  *((volatile uint32_t *) 0x51100604) = 0x000e800a; // Rows
  *((volatile uint32_t *) 0x51100608) = 0x0002800a; // Columns
  *((volatile uint32_t *) 0x51100610) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x51100614) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x51100618) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5110061c) = 0x00001800; // SRAM write ptr
  *((volatile uint32_t *) 0x51100624) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100628) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x5110062c) = 0x00001400; // SRAM read ptr
  *((volatile uint32_t *) 0x51100630) = 0x0088eba0; // Layer control
  *((volatile uint32_t *) 0x51100634) = 0x001b8010; // Layer control 2
  *((volatile uint32_t *) 0x51100638) = 0x00000618; // Mask count
  *((volatile uint32_t *) 0x5110063c) = 0x000002a0; // Mask offset
  *((volatile uint32_t *) 0x51100640) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x51100644) = 0x00000005; // TRAM ptr max
  *((volatile uint32_t *) 0x5110064c) = 0x00001000; // Post processing register
  *((volatile uint32_t *) 0x51100648) = 0xffffffff; // Mask and processor enables

  // Layer 6 quadrant 1
  *((volatile uint32_t *) 0x52100604) = 0x000e800a; // Rows
  *((volatile uint32_t *) 0x52100608) = 0x0002800a; // Columns
  *((volatile uint32_t *) 0x52100610) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x52100614) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x52100618) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5210061c) = 0x00001800; // SRAM write ptr
  *((volatile uint32_t *) 0x52100624) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100628) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x5210062c) = 0x00001400; // SRAM read ptr
  *((volatile uint32_t *) 0x52100630) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x52100634) = 0x001b8010; // Layer control 2
  *((volatile uint32_t *) 0x52100638) = 0x00000618; // Mask count
  *((volatile uint32_t *) 0x5210063c) = 0x000002a0; // Mask offset
  *((volatile uint32_t *) 0x52100640) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x52100644) = 0x00000005; // TRAM ptr max
  *((volatile uint32_t *) 0x52100648) = 0xffffffff; // Mask and processor enables

  // Layer 6 quadrant 2
  *((volatile uint32_t *) 0x53100604) = 0x000e800a; // Rows
  *((volatile uint32_t *) 0x53100608) = 0x0002800a; // Columns
  *((volatile uint32_t *) 0x53100610) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x53100614) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x53100618) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5310061c) = 0x00001800; // SRAM write ptr
  *((volatile uint32_t *) 0x53100624) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100628) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x5310062c) = 0x00001400; // SRAM read ptr
  *((volatile uint32_t *) 0x53100630) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x53100634) = 0x001b8010; // Layer control 2
  *((volatile uint32_t *) 0x53100638) = 0x00000618; // Mask count
  *((volatile uint32_t *) 0x5310063c) = 0x000002a0; // Mask offset
  *((volatile uint32_t *) 0x53100640) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x53100644) = 0x00000005; // TRAM ptr max
  *((volatile uint32_t *) 0x53100648) = 0xffffffff; // Mask and processor enables

  // Layer 6 quadrant 3
  *((volatile uint32_t *) 0x54100604) = 0x000e800a; // Rows
  *((volatile uint32_t *) 0x54100608) = 0x0002800a; // Columns
  *((volatile uint32_t *) 0x54100610) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x54100614) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x54100618) = 0x00000021; // Stride
  *((volatile uint32_t *) 0x5410061c) = 0x00001800; // SRAM write ptr
  *((volatile uint32_t *) 0x54100624) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100628) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x5410062c) = 0x00001400; // SRAM read ptr
  *((volatile uint32_t *) 0x54100630) = 0x00880ba0; // Layer control
  *((volatile uint32_t *) 0x54100634) = 0x001b8010; // Layer control 2
  *((volatile uint32_t *) 0x54100638) = 0x00000618; // Mask count
  *((volatile uint32_t *) 0x5410063c) = 0x000002a0; // Mask offset
  *((volatile uint32_t *) 0x54100640) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x54100644) = 0x00000005; // TRAM ptr max
  *((volatile uint32_t *) 0x54100648) = 0x00ff00ff; // Mask and processor enables

  // Layer 7 quadrant 0
  *((volatile uint32_t *) 0x51100704) = 0x0001000b; // Rows
  *((volatile uint32_t *) 0x51100708) = 0x0001000b; // Columns
  *((volatile uint32_t *) 0x51100718) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5110071c) = 0x00001001; // SRAM write ptr
  *((volatile uint32_t *) 0x51100720) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x51100724) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110072c) = 0x00001400; // SRAM read ptr
  *((volatile uint32_t *) 0x51100730) = 0x0000e920; // Layer control
  *((volatile uint32_t *) 0x51100734) = 0x001b8010; // Layer control 2
  *((volatile uint32_t *) 0x51100738) = 0x000038d8; // Mask count
  *((volatile uint32_t *) 0x5110073c) = 0x00003720; // Mask offset
  *((volatile uint32_t *) 0x51100740) = 0x00000037; // Output channel count
  *((volatile uint32_t *) 0x5110070c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5110074c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x51100748) = 0xffffffff; // Mask and processor enables

  // Layer 7 quadrant 1
  *((volatile uint32_t *) 0x52100704) = 0x0001000b; // Rows
  *((volatile uint32_t *) 0x52100708) = 0x0001000b; // Columns
  *((volatile uint32_t *) 0x52100718) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5210071c) = 0x00001001; // SRAM write ptr
  *((volatile uint32_t *) 0x52100720) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x52100724) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210072c) = 0x00001400; // SRAM read ptr
  *((volatile uint32_t *) 0x52100730) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x52100734) = 0x001b8010; // Layer control 2
  *((volatile uint32_t *) 0x52100738) = 0x000038d8; // Mask count
  *((volatile uint32_t *) 0x5210073c) = 0x00003720; // Mask offset
  *((volatile uint32_t *) 0x52100740) = 0x00000037; // Output channel count
  *((volatile uint32_t *) 0x5210070c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5210074c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x52100748) = 0xffffffff; // Mask and processor enables

  // Layer 7 quadrant 2
  *((volatile uint32_t *) 0x53100704) = 0x0001000b; // Rows
  *((volatile uint32_t *) 0x53100708) = 0x0001000b; // Columns
  *((volatile uint32_t *) 0x53100718) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5310071c) = 0x00001001; // SRAM write ptr
  *((volatile uint32_t *) 0x53100720) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x53100724) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310072c) = 0x00001400; // SRAM read ptr
  *((volatile uint32_t *) 0x53100730) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x53100734) = 0x001b8010; // Layer control 2
  *((volatile uint32_t *) 0x53100738) = 0x000038d8; // Mask count
  *((volatile uint32_t *) 0x5310073c) = 0x00003720; // Mask offset
  *((volatile uint32_t *) 0x53100740) = 0x00000037; // Output channel count
  *((volatile uint32_t *) 0x5310070c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5310074c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x53100748) = 0xffffffff; // Mask and processor enables

  // Layer 7 quadrant 3
  *((volatile uint32_t *) 0x54100704) = 0x0001000b; // Rows
  *((volatile uint32_t *) 0x54100708) = 0x0001000b; // Columns
  *((volatile uint32_t *) 0x54100718) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5410071c) = 0x00001001; // SRAM write ptr
  *((volatile uint32_t *) 0x54100720) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x54100724) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410072c) = 0x00001400; // SRAM read ptr
  *((volatile uint32_t *) 0x54100730) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x54100734) = 0x001b8010; // Layer control 2
  *((volatile uint32_t *) 0x54100738) = 0x000038d8; // Mask count
  *((volatile uint32_t *) 0x5410073c) = 0x00003720; // Mask offset
  *((volatile uint32_t *) 0x54100740) = 0x00000037; // Output channel count
  *((volatile uint32_t *) 0x5410070c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5410074c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x54100748) = 0x00ff00ff; // Mask and processor enables

  // Layer 8 quadrant 0
  *((volatile uint32_t *) 0x51100804) = 0x0002800b; // Rows
  *((volatile uint32_t *) 0x51100808) = 0x0001800b; // Columns
  *((volatile uint32_t *) 0x51100818) = 0x00000020; // Stride
  *((volatile uint32_t *) 0x5110081c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x51100824) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110082c) = 0x00001800; // SRAM read ptr
  *((volatile uint32_t *) 0x51100830) = 0x0088e920; // Layer control
  *((volatile uint32_t *) 0x51100834) = 0x001b8011; // Layer control 2
  *((volatile uint32_t *) 0x51100838) = 0x000009d8; // Mask count
  *((volatile uint32_t *) 0x5110083c) = 0x00000660; // Mask offset
  *((volatile uint32_t *) 0x51100840) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x51100844) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5110084c) = 0x10004000; // Post processing register
  *((volatile uint32_t *) 0x51100848) = 0xffffffff; // Mask and processor enables

  // Layer 8 quadrant 1
  *((volatile uint32_t *) 0x52100804) = 0x0002800b; // Rows
  *((volatile uint32_t *) 0x52100808) = 0x0001800b; // Columns
  *((volatile uint32_t *) 0x52100818) = 0x00000020; // Stride
  *((volatile uint32_t *) 0x5210081c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x52100824) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210082c) = 0x00001800; // SRAM read ptr
  *((volatile uint32_t *) 0x52100830) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x52100834) = 0x001b8011; // Layer control 2
  *((volatile uint32_t *) 0x52100838) = 0x000009d8; // Mask count
  *((volatile uint32_t *) 0x5210083c) = 0x00000660; // Mask offset
  *((volatile uint32_t *) 0x52100840) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x52100844) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5210084c) = 0x10005080; // Post processing register
  *((volatile uint32_t *) 0x52100848) = 0xffffffff; // Mask and processor enables

  // Layer 8 quadrant 2
  *((volatile uint32_t *) 0x53100804) = 0x0002800b; // Rows
  *((volatile uint32_t *) 0x53100808) = 0x0001800b; // Columns
  *((volatile uint32_t *) 0x53100818) = 0x00000020; // Stride
  *((volatile uint32_t *) 0x5310081c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x53100824) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310082c) = 0x00001800; // SRAM read ptr
  *((volatile uint32_t *) 0x53100830) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x53100834) = 0x001b8011; // Layer control 2
  *((volatile uint32_t *) 0x53100838) = 0x000009d8; // Mask count
  *((volatile uint32_t *) 0x5310083c) = 0x00000660; // Mask offset
  *((volatile uint32_t *) 0x53100840) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x53100844) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5310084c) = 0x10004000; // Post processing register
  *((volatile uint32_t *) 0x53100848) = 0xffffffff; // Mask and processor enables

  // Layer 8 quadrant 3
  *((volatile uint32_t *) 0x54100804) = 0x0002800b; // Rows
  *((volatile uint32_t *) 0x54100808) = 0x0001800b; // Columns
  *((volatile uint32_t *) 0x54100818) = 0x00000020; // Stride
  *((volatile uint32_t *) 0x5410081c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x54100824) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410082c) = 0x00001800; // SRAM read ptr
  *((volatile uint32_t *) 0x54100830) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x54100834) = 0x001b8011; // Layer control 2
  *((volatile uint32_t *) 0x54100838) = 0x000009d8; // Mask count
  *((volatile uint32_t *) 0x5410083c) = 0x00000660; // Mask offset
  *((volatile uint32_t *) 0x54100840) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x54100844) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5410084c) = 0x10004000; // Post processing register
  *((volatile uint32_t *) 0x54100848) = 0x00ff00ff; // Mask and processor enables

  // Layer 9 quadrant 0
  *((volatile uint32_t *) 0x51100904) = 0x0002800b; // Rows
  *((volatile uint32_t *) 0x51100908) = 0x0001800b; // Columns
  *((volatile uint32_t *) 0x51100918) = 0x00000020; // Stride
  *((volatile uint32_t *) 0x5110091c) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x51100924) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110092c) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x51100930) = 0x0088eb20; // Layer control
  *((volatile uint32_t *) 0x51100934) = 0x001b8001; // Layer control 2
  *((volatile uint32_t *) 0x51100938) = 0x00000d58; // Mask count
  *((volatile uint32_t *) 0x5110093c) = 0x000009e0; // Mask offset
  *((volatile uint32_t *) 0x51100940) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x51100944) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5110094c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x51100948) = 0xffffffff; // Mask and processor enables

  // Layer 9 quadrant 1
  *((volatile uint32_t *) 0x52100904) = 0x0002800b; // Rows
  *((volatile uint32_t *) 0x52100908) = 0x0001800b; // Columns
  *((volatile uint32_t *) 0x52100918) = 0x00000020; // Stride
  *((volatile uint32_t *) 0x5210091c) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x52100924) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210092c) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x52100930) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x52100934) = 0x001b8001; // Layer control 2
  *((volatile uint32_t *) 0x52100938) = 0x00000d58; // Mask count
  *((volatile uint32_t *) 0x5210093c) = 0x000009e0; // Mask offset
  *((volatile uint32_t *) 0x52100940) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x52100944) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5210094c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x52100948) = 0xffffffff; // Mask and processor enables

  // Layer 9 quadrant 2
  *((volatile uint32_t *) 0x53100904) = 0x0002800b; // Rows
  *((volatile uint32_t *) 0x53100908) = 0x0001800b; // Columns
  *((volatile uint32_t *) 0x53100918) = 0x00000020; // Stride
  *((volatile uint32_t *) 0x5310091c) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x53100924) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310092c) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x53100930) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x53100934) = 0x001b8001; // Layer control 2
  *((volatile uint32_t *) 0x53100938) = 0x00000d58; // Mask count
  *((volatile uint32_t *) 0x5310093c) = 0x000009e0; // Mask offset
  *((volatile uint32_t *) 0x53100940) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x53100944) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5310094c) = 0x00023080; // Post processing register
  *((volatile uint32_t *) 0x53100948) = 0xffffffff; // Mask and processor enables

  // Layer 9 quadrant 3
  *((volatile uint32_t *) 0x54100904) = 0x0002800b; // Rows
  *((volatile uint32_t *) 0x54100908) = 0x0001800b; // Columns
  *((volatile uint32_t *) 0x54100918) = 0x00000020; // Stride
  *((volatile uint32_t *) 0x5410091c) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x54100924) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410092c) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x54100930) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x54100934) = 0x001b8001; // Layer control 2
  *((volatile uint32_t *) 0x54100938) = 0x00000d58; // Mask count
  *((volatile uint32_t *) 0x5410093c) = 0x000009e0; // Mask offset
  *((volatile uint32_t *) 0x54100940) = 0x0000006f; // Output channel count
  *((volatile uint32_t *) 0x54100944) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x5410094c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x54100948) = 0x00ff00ff; // Mask and processor enables

  // Layer 10 quadrant 0
  *((volatile uint32_t *) 0x51100a04) = 0x00018017; // Rows
  *((volatile uint32_t *) 0x51100a08) = 0x00018017; // Columns
  *((volatile uint32_t *) 0x51100a18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x51100a24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100a2c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x51100a30) = 0x0088e920; // Layer control
  *((volatile uint32_t *) 0x51100a34) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x51100a38) = 0x00000e38; // Mask count
  *((volatile uint32_t *) 0x51100a3c) = 0x00000d60; // Mask offset
  *((volatile uint32_t *) 0x51100a40) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x51100a44) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x51100a4c) = 0x10004000; // Post processing register
  *((volatile uint32_t *) 0x51100a48) = 0xffffffff; // Mask and processor enables

  // Layer 10 quadrant 1
  *((volatile uint32_t *) 0x52100a04) = 0x00018017; // Rows
  *((volatile uint32_t *) 0x52100a08) = 0x00018017; // Columns
  *((volatile uint32_t *) 0x52100a18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x52100a24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100a2c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x52100a30) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x52100a34) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x52100a38) = 0x00000e38; // Mask count
  *((volatile uint32_t *) 0x52100a3c) = 0x00000d60; // Mask offset
  *((volatile uint32_t *) 0x52100a40) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x52100a44) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x52100a4c) = 0x10004000; // Post processing register
  *((volatile uint32_t *) 0x52100a48) = 0xffffffff; // Mask and processor enables

  // Layer 10 quadrant 2
  *((volatile uint32_t *) 0x53100a04) = 0x00018017; // Rows
  *((volatile uint32_t *) 0x53100a08) = 0x00018017; // Columns
  *((volatile uint32_t *) 0x53100a18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x53100a24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100a2c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x53100a30) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x53100a34) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x53100a38) = 0x00000e38; // Mask count
  *((volatile uint32_t *) 0x53100a3c) = 0x00000d60; // Mask offset
  *((volatile uint32_t *) 0x53100a40) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x53100a44) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x53100a4c) = 0x100050b8; // Post processing register
  *((volatile uint32_t *) 0x53100a48) = 0xffffffff; // Mask and processor enables

  // Layer 10 quadrant 3
  *((volatile uint32_t *) 0x54100a04) = 0x00018017; // Rows
  *((volatile uint32_t *) 0x54100a08) = 0x00018017; // Columns
  *((volatile uint32_t *) 0x54100a18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x54100a24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100a2c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x54100a30) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x54100a34) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x54100a38) = 0x00000e38; // Mask count
  *((volatile uint32_t *) 0x54100a3c) = 0x00000d60; // Mask offset
  *((volatile uint32_t *) 0x54100a40) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x54100a44) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x54100a4c) = 0x10004000; // Post processing register
  *((volatile uint32_t *) 0x54100a48) = 0x00ff00ff; // Mask and processor enables

  // Layer 11 quadrant 0
  *((volatile uint32_t *) 0x51100b04) = 0x00018017; // Rows
  *((volatile uint32_t *) 0x51100b08) = 0x00018017; // Columns
  *((volatile uint32_t *) 0x51100b18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x51100b1c) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x51100b24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100b30) = 0x0088eb20; // Layer control
  *((volatile uint32_t *) 0x51100b34) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x51100b38) = 0x00000f18; // Mask count
  *((volatile uint32_t *) 0x51100b3c) = 0x00000e40; // Mask offset
  *((volatile uint32_t *) 0x51100b40) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x51100b44) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x51100b4c) = 0x000230c8; // Post processing register
  *((volatile uint32_t *) 0x51100b48) = 0xffffffff; // Mask and processor enables

  // Layer 11 quadrant 1
  *((volatile uint32_t *) 0x52100b04) = 0x00018017; // Rows
  *((volatile uint32_t *) 0x52100b08) = 0x00018017; // Columns
  *((volatile uint32_t *) 0x52100b18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x52100b1c) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x52100b24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100b30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x52100b34) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x52100b38) = 0x00000f18; // Mask count
  *((volatile uint32_t *) 0x52100b3c) = 0x00000e40; // Mask offset
  *((volatile uint32_t *) 0x52100b40) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x52100b44) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x52100b4c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x52100b48) = 0xffffffff; // Mask and processor enables

  // Layer 11 quadrant 2
  *((volatile uint32_t *) 0x53100b04) = 0x00018017; // Rows
  *((volatile uint32_t *) 0x53100b08) = 0x00018017; // Columns
  *((volatile uint32_t *) 0x53100b18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x53100b1c) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x53100b24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100b30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x53100b34) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x53100b38) = 0x00000f18; // Mask count
  *((volatile uint32_t *) 0x53100b3c) = 0x00000e40; // Mask offset
  *((volatile uint32_t *) 0x53100b40) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x53100b44) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x53100b4c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x53100b48) = 0xffffffff; // Mask and processor enables

  // Layer 11 quadrant 3
  *((volatile uint32_t *) 0x54100b04) = 0x00018017; // Rows
  *((volatile uint32_t *) 0x54100b08) = 0x00018017; // Columns
  *((volatile uint32_t *) 0x54100b18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x54100b1c) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x54100b24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100b30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x54100b34) = 0x000d8000; // Layer control 2
  *((volatile uint32_t *) 0x54100b38) = 0x00000f18; // Mask count
  *((volatile uint32_t *) 0x54100b3c) = 0x00000e40; // Mask offset
  *((volatile uint32_t *) 0x54100b40) = 0x0000001b; // Output channel count
  *((volatile uint32_t *) 0x54100b44) = 0x00000017; // TRAM ptr max
  *((volatile uint32_t *) 0x54100b4c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x54100b48) = 0x00ff00ff; // Mask and processor enables

  // Layer 12 quadrant 0
  *((volatile uint32_t *) 0x51100c04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x51100c08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x51100c18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x51100c1c) = 0x00060000; // SRAM write ptr
  *((volatile uint32_t *) 0x51100c24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100c2c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x51100c30) = 0x00882920; // Layer control
  *((volatile uint32_t *) 0x51100c34) = 0x00038000; // Layer control 2
  *((volatile uint32_t *) 0x51100c38) = 0x00000118; // Mask count
  *((volatile uint32_t *) 0x51100c3c) = 0x000000e0; // Mask offset
  *((volatile uint32_t *) 0x51100c40) = 0x00000007; // Output channel count
  *((volatile uint32_t *) 0x51100c44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x51100c4c) = 0x10002000; // Post processing register
  *((volatile uint32_t *) 0x51100c48) = 0xffffffff; // Mask and processor enables

  // Layer 12 quadrant 1
  *((volatile uint32_t *) 0x52100c04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x52100c08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x52100c18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x52100c1c) = 0x00060000; // SRAM write ptr
  *((volatile uint32_t *) 0x52100c24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100c2c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x52100c30) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x52100c34) = 0x00038000; // Layer control 2
  *((volatile uint32_t *) 0x52100c38) = 0x00000118; // Mask count
  *((volatile uint32_t *) 0x52100c3c) = 0x000000e0; // Mask offset
  *((volatile uint32_t *) 0x52100c40) = 0x00000007; // Output channel count
  *((volatile uint32_t *) 0x52100c44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x52100c4c) = 0x100030d4; // Post processing register
  *((volatile uint32_t *) 0x52100c48) = 0x0fff0fff; // Mask and processor enables

  // Layer 12 quadrant 2
  *((volatile uint32_t *) 0x53100c04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x53100c08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x53100c18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x53100c1c) = 0x00060000; // SRAM write ptr
  *((volatile uint32_t *) 0x53100c24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100c2c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x53100c30) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x53100c34) = 0x00038000; // Layer control 2
  *((volatile uint32_t *) 0x53100c38) = 0x00000118; // Mask count
  *((volatile uint32_t *) 0x53100c3c) = 0x000000e0; // Mask offset
  *((volatile uint32_t *) 0x53100c40) = 0x00000007; // Output channel count
  *((volatile uint32_t *) 0x53100c44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x53100c4c) = 0x10002000; // Post processing register

  // Layer 12 quadrant 3
  *((volatile uint32_t *) 0x54100c04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x54100c08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x54100c18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x54100c1c) = 0x00060000; // SRAM write ptr
  *((volatile uint32_t *) 0x54100c24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100c2c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x54100c30) = 0x00880920; // Layer control
  *((volatile uint32_t *) 0x54100c34) = 0x00038000; // Layer control 2
  *((volatile uint32_t *) 0x54100c38) = 0x00000118; // Mask count
  *((volatile uint32_t *) 0x54100c3c) = 0x000000e0; // Mask offset
  *((volatile uint32_t *) 0x54100c40) = 0x00000007; // Output channel count
  *((volatile uint32_t *) 0x54100c44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x54100c4c) = 0x10002000; // Post processing register

  // Layer 13 quadrant 0
  *((volatile uint32_t *) 0x51100d04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x51100d08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x51100d18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x51100d1c) = 0x000001c0; // SRAM write ptr
  *((volatile uint32_t *) 0x51100d24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100d30) = 0x00888b20; // Layer control
  *((volatile uint32_t *) 0x51100d34) = 0x00178000; // Layer control 2
  *((volatile uint32_t *) 0x51100d38) = 0x00001098; // Mask count
  *((volatile uint32_t *) 0x51100d3c) = 0x00000f20; // Mask offset
  *((volatile uint32_t *) 0x51100d40) = 0x0000002f; // Output channel count
  *((volatile uint32_t *) 0x51100d44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x51100d4c) = 0x00002000; // Post processing register

  // Layer 13 quadrant 1
  *((volatile uint32_t *) 0x52100d04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x52100d08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x52100d18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x52100d1c) = 0x000001c0; // SRAM write ptr
  *((volatile uint32_t *) 0x52100d24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100d30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x52100d34) = 0x00178000; // Layer control 2
  *((volatile uint32_t *) 0x52100d38) = 0x00001098; // Mask count
  *((volatile uint32_t *) 0x52100d3c) = 0x00000f20; // Mask offset
  *((volatile uint32_t *) 0x52100d40) = 0x0000002f; // Output channel count
  *((volatile uint32_t *) 0x52100d44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x52100d4c) = 0x00002000; // Post processing register

  // Layer 13 quadrant 2
  *((volatile uint32_t *) 0x53100d04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x53100d08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x53100d18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x53100d1c) = 0x000001c0; // SRAM write ptr
  *((volatile uint32_t *) 0x53100d24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100d30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x53100d34) = 0x00178000; // Layer control 2
  *((volatile uint32_t *) 0x53100d38) = 0x00001098; // Mask count
  *((volatile uint32_t *) 0x53100d3c) = 0x00000f20; // Mask offset
  *((volatile uint32_t *) 0x53100d40) = 0x0000002f; // Output channel count
  *((volatile uint32_t *) 0x53100d44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x53100d4c) = 0x00002000; // Post processing register

  // Layer 13 quadrant 3
  *((volatile uint32_t *) 0x54100d04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x54100d08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x54100d18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x54100d1c) = 0x000001c0; // SRAM write ptr
  *((volatile uint32_t *) 0x54100d24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100d30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x54100d34) = 0x00178000; // Layer control 2
  *((volatile uint32_t *) 0x54100d38) = 0x00001098; // Mask count
  *((volatile uint32_t *) 0x54100d3c) = 0x00000f20; // Mask offset
  *((volatile uint32_t *) 0x54100d40) = 0x0000002f; // Output channel count
  *((volatile uint32_t *) 0x54100d44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x54100d4c) = 0x00003080; // Post processing register
  *((volatile uint32_t *) 0x54100d48) = 0xffffffff; // Mask and processor enables

  // Layer 14 quadrant 0
  *((volatile uint32_t *) 0x51100e04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x51100e08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x51100e18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x51100e1c) = 0x00000154; // SRAM write ptr
  *((volatile uint32_t *) 0x51100e24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100e2c) = 0x000001c0; // SRAM read ptr
  *((volatile uint32_t *) 0x51100e30) = 0x0088eb20; // Layer control
  *((volatile uint32_t *) 0x51100e34) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x51100e38) = 0x00001118; // Mask count
  *((volatile uint32_t *) 0x51100e3c) = 0x00000f20; // Mask offset
  *((volatile uint32_t *) 0x51100e40) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x51100e44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x51100e48) = 0xffffffff; // Mask and processor enables

  // Layer 14 quadrant 1
  *((volatile uint32_t *) 0x52100e04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x52100e08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x52100e18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x52100e1c) = 0x00000154; // SRAM write ptr
  *((volatile uint32_t *) 0x52100e24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100e2c) = 0x000001c0; // SRAM read ptr
  *((volatile uint32_t *) 0x52100e30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x52100e34) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x52100e38) = 0x00001118; // Mask count
  *((volatile uint32_t *) 0x52100e3c) = 0x00000f20; // Mask offset
  *((volatile uint32_t *) 0x52100e40) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x52100e44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x52100e48) = 0xffffffff; // Mask and processor enables

  // Layer 14 quadrant 2
  *((volatile uint32_t *) 0x53100e04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x53100e08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x53100e18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x53100e1c) = 0x00000154; // SRAM write ptr
  *((volatile uint32_t *) 0x53100e24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100e2c) = 0x000001c0; // SRAM read ptr
  *((volatile uint32_t *) 0x53100e30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x53100e34) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x53100e38) = 0x00001118; // Mask count
  *((volatile uint32_t *) 0x53100e3c) = 0x00000f20; // Mask offset
  *((volatile uint32_t *) 0x53100e40) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x53100e44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x53100e48) = 0xffffffff; // Mask and processor enables

  // Layer 14 quadrant 3
  *((volatile uint32_t *) 0x54100e04) = 0x0001802f; // Rows
  *((volatile uint32_t *) 0x54100e08) = 0x0001802f; // Columns
  *((volatile uint32_t *) 0x54100e18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x54100e1c) = 0x00000154; // SRAM write ptr
  *((volatile uint32_t *) 0x54100e24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100e2c) = 0x000001c0; // SRAM read ptr
  *((volatile uint32_t *) 0x54100e30) = 0x00880b20; // Layer control
  *((volatile uint32_t *) 0x54100e34) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x54100e38) = 0x00001118; // Mask count
  *((volatile uint32_t *) 0x54100e3c) = 0x00000f20; // Mask offset
  *((volatile uint32_t *) 0x54100e40) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x54100e44) = 0x0000002f; // TRAM ptr max
  *((volatile uint32_t *) 0x54100e4c) = 0x00001000; // Post processing register

  // Layer 15 quadrant 0
  *((volatile uint32_t *) 0x51100f04) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x51100f08) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x51100f18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x51100f1c) = 0x00000100; // SRAM write ptr
  *((volatile uint32_t *) 0x51100f20) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x51100f24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x51100f2c) = 0x00000154; // SRAM read ptr
  *((volatile uint32_t *) 0x51100f30) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x51100f34) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x51100f38) = 0x00009c18; // Mask count
  *((volatile uint32_t *) 0x51100f3c) = 0x00009a20; // Mask offset
  *((volatile uint32_t *) 0x51100f40) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x51100f0c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x51100f4c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x51100f48) = 0xffffffff; // Mask and processor enables

  // Layer 15 quadrant 1
  *((volatile uint32_t *) 0x52100f04) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x52100f08) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x52100f18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x52100f1c) = 0x00000100; // SRAM write ptr
  *((volatile uint32_t *) 0x52100f20) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x52100f24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x52100f2c) = 0x00000154; // SRAM read ptr
  *((volatile uint32_t *) 0x52100f30) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x52100f34) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x52100f38) = 0x00009c18; // Mask count
  *((volatile uint32_t *) 0x52100f3c) = 0x00009a20; // Mask offset
  *((volatile uint32_t *) 0x52100f40) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x52100f0c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x52100f4c) = 0x00003040; // Post processing register
  *((volatile uint32_t *) 0x52100f48) = 0xffffffff; // Mask and processor enables

  // Layer 15 quadrant 2
  *((volatile uint32_t *) 0x53100f04) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x53100f08) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x53100f18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x53100f1c) = 0x00000100; // SRAM write ptr
  *((volatile uint32_t *) 0x53100f20) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x53100f24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x53100f2c) = 0x00000154; // SRAM read ptr
  *((volatile uint32_t *) 0x53100f30) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x53100f34) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x53100f38) = 0x00009c18; // Mask count
  *((volatile uint32_t *) 0x53100f3c) = 0x00009a20; // Mask offset
  *((volatile uint32_t *) 0x53100f40) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x53100f0c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x53100f4c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x53100f48) = 0xffffffff; // Mask and processor enables

  // Layer 15 quadrant 3
  *((volatile uint32_t *) 0x54100f04) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x54100f08) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x54100f18) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x54100f1c) = 0x00000100; // SRAM write ptr
  *((volatile uint32_t *) 0x54100f20) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x54100f24) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x54100f2c) = 0x00000154; // SRAM read ptr
  *((volatile uint32_t *) 0x54100f30) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x54100f34) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x54100f38) = 0x00009c18; // Mask count
  *((volatile uint32_t *) 0x54100f3c) = 0x00009a20; // Mask offset
  *((volatile uint32_t *) 0x54100f40) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x54100f0c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x54100f4c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x54100f48) = 0xffffffff; // Mask and processor enables

  // Layer 16 quadrant 0
  *((volatile uint32_t *) 0x51101004) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x51101008) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x51101018) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5110101c) = 0x00000094; // SRAM write ptr
  *((volatile uint32_t *) 0x51101020) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x51101024) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110102c) = 0x00000100; // SRAM read ptr
  *((volatile uint32_t *) 0x51101030) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x51101034) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x51101038) = 0x00009e58; // Mask count
  *((volatile uint32_t *) 0x5110103c) = 0x00009c60; // Mask offset
  *((volatile uint32_t *) 0x51101040) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5110100c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5110104c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x51101048) = 0xffffffff; // Mask and processor enables

  // Layer 16 quadrant 1
  *((volatile uint32_t *) 0x52101004) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x52101008) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x52101018) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5210101c) = 0x00000094; // SRAM write ptr
  *((volatile uint32_t *) 0x52101020) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x52101024) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210102c) = 0x00000100; // SRAM read ptr
  *((volatile uint32_t *) 0x52101030) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x52101034) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x52101038) = 0x00009e58; // Mask count
  *((volatile uint32_t *) 0x5210103c) = 0x00009c60; // Mask offset
  *((volatile uint32_t *) 0x52101040) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5210100c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5210104c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x52101048) = 0xffffffff; // Mask and processor enables

  // Layer 16 quadrant 2
  *((volatile uint32_t *) 0x53101004) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x53101008) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x53101018) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5310101c) = 0x00000094; // SRAM write ptr
  *((volatile uint32_t *) 0x53101020) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x53101024) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310102c) = 0x00000100; // SRAM read ptr
  *((volatile uint32_t *) 0x53101030) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x53101034) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x53101038) = 0x00009e58; // Mask count
  *((volatile uint32_t *) 0x5310103c) = 0x00009c60; // Mask offset
  *((volatile uint32_t *) 0x53101040) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5310100c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5310104c) = 0x00003040; // Post processing register
  *((volatile uint32_t *) 0x53101048) = 0xffffffff; // Mask and processor enables

  // Layer 16 quadrant 3
  *((volatile uint32_t *) 0x54101004) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x54101008) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x54101018) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5410101c) = 0x00000094; // SRAM write ptr
  *((volatile uint32_t *) 0x54101020) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x54101024) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410102c) = 0x00000100; // SRAM read ptr
  *((volatile uint32_t *) 0x54101030) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x54101034) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x54101038) = 0x00009e58; // Mask count
  *((volatile uint32_t *) 0x5410103c) = 0x00009c60; // Mask offset
  *((volatile uint32_t *) 0x54101040) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5410100c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5410104c) = 0x00002000; // Post processing register
  *((volatile uint32_t *) 0x54101048) = 0xffffffff; // Mask and processor enables

  // Layer 17 quadrant 0
  *((volatile uint32_t *) 0x51101104) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x51101108) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x51101118) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5110111c) = 0x00000040; // SRAM write ptr
  *((volatile uint32_t *) 0x51101120) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x51101124) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110112c) = 0x00000094; // SRAM read ptr
  *((volatile uint32_t *) 0x51101130) = 0x0000e920; // Layer control
  *((volatile uint32_t *) 0x51101134) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x51101138) = 0x0000a098; // Mask count
  *((volatile uint32_t *) 0x5110113c) = 0x00009ea0; // Mask offset
  *((volatile uint32_t *) 0x51101140) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5110110c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5110114c) = 0x00004000; // Post processing register
  *((volatile uint32_t *) 0x51101148) = 0xffffffff; // Mask and processor enables

  // Layer 17 quadrant 1
  *((volatile uint32_t *) 0x52101104) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x52101108) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x52101118) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5210111c) = 0x00000040; // SRAM write ptr
  *((volatile uint32_t *) 0x52101120) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x52101124) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210112c) = 0x00000094; // SRAM read ptr
  *((volatile uint32_t *) 0x52101130) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x52101134) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x52101138) = 0x0000a098; // Mask count
  *((volatile uint32_t *) 0x5210113c) = 0x00009ea0; // Mask offset
  *((volatile uint32_t *) 0x52101140) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5210110c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5210114c) = 0x00004000; // Post processing register
  *((volatile uint32_t *) 0x52101148) = 0xffffffff; // Mask and processor enables

  // Layer 17 quadrant 2
  *((volatile uint32_t *) 0x53101104) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x53101108) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x53101118) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5310111c) = 0x00000040; // SRAM write ptr
  *((volatile uint32_t *) 0x53101120) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x53101124) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310112c) = 0x00000094; // SRAM read ptr
  *((volatile uint32_t *) 0x53101130) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x53101134) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x53101138) = 0x0000a098; // Mask count
  *((volatile uint32_t *) 0x5310113c) = 0x00009ea0; // Mask offset
  *((volatile uint32_t *) 0x53101140) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5310110c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5310114c) = 0x00004000; // Post processing register
  *((volatile uint32_t *) 0x53101148) = 0xffffffff; // Mask and processor enables

  // Layer 17 quadrant 3
  *((volatile uint32_t *) 0x54101104) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x54101108) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x54101118) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x5410111c) = 0x00000040; // SRAM write ptr
  *((volatile uint32_t *) 0x54101120) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x54101124) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410112c) = 0x00000094; // SRAM read ptr
  *((volatile uint32_t *) 0x54101130) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x54101134) = 0x001f8000; // Layer control 2
  *((volatile uint32_t *) 0x54101138) = 0x0000a098; // Mask count
  *((volatile uint32_t *) 0x5410113c) = 0x00009ea0; // Mask offset
  *((volatile uint32_t *) 0x54101140) = 0x0000003f; // Output channel count
  *((volatile uint32_t *) 0x5410110c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5410114c) = 0x00005040; // Post processing register
  *((volatile uint32_t *) 0x54101148) = 0xffffffff; // Mask and processor enables

  // Layer 18 quadrant 0
  *((volatile uint32_t *) 0x51101204) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x51101208) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x51101218) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x51101220) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x51101224) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5110122c) = 0x00000040; // SRAM read ptr
  *((volatile uint32_t *) 0x51101230) = 0x0000e920; // Layer control
  *((volatile uint32_t *) 0x51101234) = 0x000f8000; // Layer control 2
  *((volatile uint32_t *) 0x51101238) = 0x0000a1d8; // Mask count
  *((volatile uint32_t *) 0x5110123c) = 0x0000a0e0; // Mask offset
  *((volatile uint32_t *) 0x51101240) = 0x0000001f; // Output channel count
  *((volatile uint32_t *) 0x5110120c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5110124c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x51101248) = 0xffffffff; // Mask and processor enables

  // Layer 18 quadrant 1
  *((volatile uint32_t *) 0x52101204) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x52101208) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x52101218) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x52101220) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x52101224) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5210122c) = 0x00000040; // SRAM read ptr
  *((volatile uint32_t *) 0x52101230) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x52101234) = 0x000f8000; // Layer control 2
  *((volatile uint32_t *) 0x52101238) = 0x0000a1d8; // Mask count
  *((volatile uint32_t *) 0x5210123c) = 0x0000a0e0; // Mask offset
  *((volatile uint32_t *) 0x52101240) = 0x0000001f; // Output channel count
  *((volatile uint32_t *) 0x5210120c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5210124c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x52101248) = 0xffffffff; // Mask and processor enables

  // Layer 18 quadrant 2
  *((volatile uint32_t *) 0x53101204) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x53101208) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x53101218) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x53101220) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x53101224) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5310122c) = 0x00000040; // SRAM read ptr
  *((volatile uint32_t *) 0x53101230) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x53101234) = 0x000f8000; // Layer control 2
  *((volatile uint32_t *) 0x53101238) = 0x0000a1d8; // Mask count
  *((volatile uint32_t *) 0x5310123c) = 0x0000a0e0; // Mask offset
  *((volatile uint32_t *) 0x53101240) = 0x0000001f; // Output channel count
  *((volatile uint32_t *) 0x5310120c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5310124c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x53101248) = 0xffffffff; // Mask and processor enables

  // Layer 18 quadrant 3
  *((volatile uint32_t *) 0x54101204) = 0x0001002f; // Rows
  *((volatile uint32_t *) 0x54101208) = 0x0001002f; // Columns
  *((volatile uint32_t *) 0x54101218) = 0x00000010; // Stride
  *((volatile uint32_t *) 0x54101220) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x54101224) = 0x00008000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5410122c) = 0x00000040; // SRAM read ptr
  *((volatile uint32_t *) 0x54101230) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x54101234) = 0x000f8000; // Layer control 2
  *((volatile uint32_t *) 0x54101238) = 0x0000a1d8; // Mask count
  *((volatile uint32_t *) 0x5410123c) = 0x0000a0e0; // Mask offset
  *((volatile uint32_t *) 0x54101240) = 0x0000001f; // Output channel count
  *((volatile uint32_t *) 0x5410120c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x5410124c) = 0x000230b0; // Post processing register
  *((volatile uint32_t *) 0x54101248) = 0xffffffff; // Mask and processor enables


  return CNN_OK;
}

int cnn_start(void)
{
  cnn_time = 0;

  *((volatile uint32_t *) 0x51000000) = 0x0010080e; // Enable quadrant 0
  *((volatile uint32_t *) 0x52000000) = 0x0010080f; // Enable quadrant 1
  *((volatile uint32_t *) 0x53000000) = 0x0010080f; // Enable quadrant 2
  *((volatile uint32_t *) 0x54000000) = 0x0010080f; // Enable quadrant 3

#ifdef CNN_INFERENCE_TIMER
  MXC_TMR_SW_Start(CNN_INFERENCE_TIMER);
#endif

  CNN_START; // Allow capture of processing time
  *((volatile uint32_t *) 0x51000000) = 0x0010000f; // Master enable quadrant 0

  return CNN_OK;
}

int cnn_enable(uint32_t clock_source, uint32_t clock_divider)
{
  // Reset all domains, restore power to CNN
  MXC_GCFR->reg3 = 0xf; // Reset
  MXC_GCFR->reg1 = 0xf; // Mask memory
  MXC_GCFR->reg0 = 0xf; // Power
  MXC_Delay(MSEC(10)); // Wait for load switches
  MXC_GCFR->reg2 = 0x0; // Iso
  MXC_GCFR->reg3 = 0x0; // Reset

  if (clock_source == MXC_S_GCR_PCLKDIV_CNNCLKSEL_IPLL)
    while ((MXC_GCR->ipll_ctrl & MXC_F_GCR_IPLL_CTRL_RDY) != MXC_F_GCR_IPLL_CTRL_RDY) ; // Wait for PLL

  MXC_GCR->pclkdiv = (MXC_GCR->pclkdiv & ~(MXC_F_GCR_PCLKDIV_CNNCLKDIV | MXC_F_GCR_PCLKDIV_CNNCLKSEL))
                     | clock_divider | clock_source;
  MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN); // Enable CNN clock

  NVIC_SetVector(CNN_IRQn, CNN_ISR); // Set CNN complete vector

  return CNN_OK;
}

int cnn_boost_enable(mxc_gpio_regs_t *port, uint32_t pin)
{
  mxc_gpio_cfg_t gpio_out;
  gpio_out.port = port;
  gpio_out.mask = pin;
  gpio_out.pad = MXC_GPIO_PAD_NONE;
  gpio_out.func = MXC_GPIO_FUNC_OUT;
  MXC_GPIO_Config(&gpio_out);
  MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);

  return CNN_OK;
}

int cnn_boost_disable(mxc_gpio_regs_t *port, uint32_t pin)
{
  mxc_gpio_cfg_t gpio_out;
  gpio_out.port = port;
  gpio_out.mask = pin;
  gpio_out.pad = MXC_GPIO_PAD_NONE;
  gpio_out.func = MXC_GPIO_FUNC_OUT;
  MXC_GPIO_Config(&gpio_out);
  MXC_GPIO_OutClr(gpio_out.port, gpio_out.mask);

  return CNN_OK;
}

int cnn_disable(void)
{
  // Disable CNN clock
  MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

  // Disable power to CNN
  MXC_GCFR->reg3 = 0xf; // Reset
  MXC_GCFR->reg2 = 0xf; // Iso
  MXC_GCFR->reg0 = 0x0; // Power
  MXC_GCFR->reg1 = 0x0; // Mask memory
  MXC_GCFR->reg3 = 0x0; // Reset

  return CNN_OK;
}

