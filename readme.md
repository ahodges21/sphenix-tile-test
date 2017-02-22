
The analysis macro has been modified so it can be run driect from the command line.

```bash
root -l -q 'sPhenixTileTest.C+("2017-01-20_21h02_10mVTrigger_FactoryBiases.dat")'
```

Breaking it down

| Command          | Action                                     |
| ---------------- | ------------------------------------------ |
| `root`           | Runs root binary                           |
| `-l`             | Hide splash screen                         |
| `-q`             | Quit after execution                       |
| `<filename>.C`   | Name of the file and function to execute   |
| `+`              | Send to ACLiC to compile                   |
| `<filename>.dat` | Date file to analyse                       |

start root
```bash
root -l
```

Load the built root file from the last part.

```cpp
TFile::Open("<rootFile>.root");
```

```cpp
.ls
```

Will list the T-Tree

```
TFile**         2017-01-20_21h02_10mVTrigger_FactoryBiases.root
 TFile*         2017-01-20_21h02_10mVTrigger_FactoryBiases.root
  KEY: TTree    rec;133 rec
  KEY: TTree    rec;132 rec
  KEY: TH1F     g1_adc;1        OH-2-01 Spectrum
  KEY: TH1F     g2_adc;1        IH-1-16 Spectrum
  KEY: TH1F     g3_adc;1        OH-1-15 Spectrum
  KEY: TH1F     g4_adc;1        IH-2-12 Spectrum
```

Then to draw any of the 

```cpp
g1_adc->Draw()
```