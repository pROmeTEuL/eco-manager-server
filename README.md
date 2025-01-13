# Eco Manager Server
## Installing
```bash
mkdir build
cd build
cmake ..
make #(or if you build it with ninja use ninja instead)
```
## API End Points
```
/api - root
/system
    ~ get all utils status
/water
    ~ get - water usage (l)
    ~ post - turn on/off
/heat
    ~ get - heater usage (m3)
    ~ post - turn on/off
/electricity
    ~ get - electricity usage (kw)
    ~ post - turn on/off
```