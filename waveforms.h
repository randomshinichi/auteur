#ifndef waveforms_h
#define waveforms_h
#define wave_samples 100
static int scaledcos[wave_samples]=
  //scaled cos, 100 samples, 0 to 4095
  {
    4095,4091,4079,4059,4031,3995,3951,3900,3842,3776,3704,3625,3540,3449,3353,3251,3145,3034,2919,2801,2680,2557,2431,2304,2176,2047,1919,1791,1664,1538,1415,1294,1176,1061,950,844,742,646,555,470,391,319,253,195,144,100,64,36,16,4,0,4,16,36,64,100,144,195,253,319,391,470,555,646,742,844,950,1061,1176,1294,1415,1538,1664,1791,1919,2047,2176,2304,2431,2557,2680,2801,2919,3034,3145,3251,3353,3449,3540,3625,3704,3776,3842,3900,3951,3995,4031,4059,4079,4091
  };

static int scaledsine[wave_samples]=	
  //scaled sine, 100 samples, 0 to 4095
  {
    2048,2176,2304,2431,2557,2680,2801,2919,3034,3145,3251,3353,3449,3540,3625,3704,3776,3842,3900,3951,3995,4031,4059,4079,4091,4095,4091,4079,4059,4031,3995,3951,3900,3842,3776,3704,3625,3540,3449,3353,3251,3145,3034,2919,2801,2680,2557,2431,2304,2176,2048,1919,1791,1664,1538,1415,1294,1176,1061,950,844,742,646,555,470,391,319,253,195,144,100,64,36,16,4,0,4,16,36,64,100,144,195,253,319,391,470,555,646,742,844,950,1061,1176,1294,1415,1538,1664,1791,1919
  };
#endif
