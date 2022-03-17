* Build aac encoder and decoder demo

```sh
% mkdir demo && cd demo
% cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_PROGRAMS=ON -DBUILD_SHARED_LIBS=OFF
% make
```

* aac encoder && decoder usage

```sh
aac-enc -r 128000 -t 5 in.wav out.aac
aac-enc out.aac out.wav
```
