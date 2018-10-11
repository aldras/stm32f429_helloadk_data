//
// i2spll_conf.c : I2S PLL configuration
//                 for STM32F4
//
// 2012/02/20 pcm1723
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define I2SPLLR_MIN (2)
#define I2SPLLR_MAX (7)

#define I2SPLLN_MIN (192)
#define I2SPLLN_MAX (432)

#define I2SDIV_MIN (2)
#define I2SDIV_MAX (511)

uint32_t pllm = 8;
//uint32_t HSE_VALUE = 8000000UL;

int32_t fs_tab[] = {
  22050,
  24000,
  32000,
  35000,
  36000,
  40000,
  44100,
  45000,
  46000,
  48000,
  49516,
  49716,
  49749,
  50000,
  60000,
  64000,
  88200,
  96000,
  100000,
  168000,
  180000
}; // int fs_tab[]

void calculate_clocks(int argc, char* argv[])
{
  int pllr, plln;
  int r_opt, n_opt, d_opt, o_opt;
  int i, fs;
  uint32_t fvco, i2sclk, tmp;
  uint32_t i2sdiv, i2sodd;
  double err, err_min, fs_opt;
  int n = (sizeof(fs_tab) / sizeof(int32_t));

  if (1 < argc) { // argument found
    if (argc > n) { // too many arguments
      argc = (n + 1); // limit to table entries
    } // if (argc > n) ...
    for (n = 1; n < argc; n++) {
// override table content by the argument
      sscanf(argv[n], "%ld", &(fs_tab[n-1]));
    } // for
    n--;
  } // if (1 < argc) ...
  printf("Target_fs  PLLN   PLLR  I2SDIV I2SODD Real_fs [Hz] error [ppm]\r\xa");
  printf("--------- ------ ------ ------ ------ ------------ -----------\r\xa");
  for (i = 0; i < n; i++) {
    fs = fs_tab[i];
    err_min = 9999;
    for (plln = I2SPLLN_MIN; plln <= I2SPLLN_MAX; plln++) {
      for (pllr = I2SPLLR_MAX; pllr >= I2SPLLR_MIN; pllr--) {
        fvco   = plln * (HSE_VALUE / pllm); // VCO output freq
        i2sclk = fvco / pllr; // i2sclk to SPI/I2S module
    if (192000000UL >= i2sclk) { // i2sclk limited to 192 MHz
          // following calculations quoted from I2S_Init() function
          /* Compute the Real divider depending on the MCLK output state, with a floating point */
          /* MCLK output is enabled */
          tmp = (uint16_t)(((((i2sclk / 256) * 10) / fs)) + 5);

          /* Remove the floating point */
          tmp = tmp / 10;

          // calculate fs error
          err = (((double)i2sclk / 256.0 / tmp / fs) - 1.0);

          /* Check the parity of the divider */
          i2sodd = (uint16_t)(tmp & (uint16_t)0x0001);

          /* Compute the i2sdiv prescaler */
          i2sdiv = (uint16_t)((tmp - i2sodd) / 2);

          /* Get the Mask for the Odd bit (SPI_I2SPR[8]) register */
          i2sodd = (uint16_t) (i2sodd << 8);

          // check if minimum error value found
          if (1 < i2sdiv) { // i2sdiv[7:0] == 0000000X forbidden
            if (fabs(err) < fabs(err_min)) { // minimum error found
              r_opt = pllr;
              n_opt = plln;
              d_opt = i2sdiv;
              o_opt = i2sodd;
              fs_opt = fs * (1.0 + err);
              err_min = err;
            } // if (err < err_min) ...
          } // if (1 < i2sdiv)
        } // if (192000000UL >= i2sclk)
      } // for (pllr
    } // for (plln
    printf("%8d %6d %6d %6d %6d %13.5f   %7.2f\r\xa", fs, n_opt, r_opt, d_opt, o_opt>>8, fs_opt, 1e6*err_min);
  } // for (i = 0;
  return;
} // int main()
