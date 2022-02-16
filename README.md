# Heltec-Wifi-Lora-V2-Helium-Mapper

## Exemple de mapper Helium avec un Heltec Wifi Lora V2 et un module GPS Neo 6M

### Cablage du Heltec et du GPS

| Heltec | Neo 6M 
|-----------|-----------|
| GND | GND |
| 3.3V | VCC |
| 22 | TX |
| 23 | RX |

## Librairies et autre

### Installer et configurer l'ESP 32

[Pour ceci suivez l'installation ici](https://github.com/helium/devdocs/blob/master/devices/arduino-quickstart/heltec-wifi-lora-32-v2.md)


[Installez la librairie lmic ibm](https://www.arduino.cc/reference/en/libraries/ibm-lmic-framework/)

[Installez la librairie TinyGPSPlus](https://www.arduino.cc/reference/en/libraries/tinygpsplus/)

## Sur la console helium

[Créez un device](https://docs.helium.com/use-the-network/console/adding-devices/)

Récuperez le DEVEUI ( en lsb, ex : 0x5D, 0x14, 0xE0, 0xBB, 0x36, 0xF9, 0x81, 0x61)

Récuperez le APPEUI ( en lsb, ex : 0x81, 0xE2, 0x50, 0x13, 0x2C, 0xF9, 0x81, 0x61)

Récuperez l'App key ( en msb, ex : 0x81, 0x74, 0x85, 0xBB, 0xC6, 0xA7, 0x81, 0x58, 0xEC, 0xEC, 0xFE, 0xFB, 0x9A, 0xC6, 0x46, 0xE4)

## Dans le fichier arduino
Et renseignez les dans le fichier arduino : 


static const u1_t PROGMEM DEVEUI[8] = { 0x5D, 0x14, 0xE0, 0xBB, 0x36, 0xF9, 0x81, 0x61 };

static const u1_t PROGMEM APPEUI[8] = { 0x81, 0xE2, 0x50, 0x13, 0x2C, 0xF9, 0x81, 0x61 };

static const u1_t PROGMEM APPKEY[16] = { 0x81, 0x74, 0x85, 0xBB, 0xC6, 0xA7, 0x81, 0x58, 0xEC, 0xEC, 0xFE, 0xFB, 0x9A, 0xC6, 0x46, 0xE4 };


## Sur la console helium

[Ajoutez une intégration du mapper helium](https://docs.helium.com/use-the-network/coverage-mapping/mappers-quickstart/)

[Créez une fonction decoder](https://docs.helium.com/use-the-network/console/functions)

Le code du decoder : 
```javascript
function Bytes2Float32(bytes) {
    var sign = (bytes & 0x80000000) ? -1 : 1;
    var exponent = ((bytes >> 23) & 0xFF) - 127;
    var significand = (bytes & ~(-1 << 23));
    if (exponent == 128)
        return sign * ((significand) ? Number.NaN : Number.POSITIVE_INFINITY);
    if (exponent == -127) {
        if (significand == 0) return sign * 0.0;
        exponent = -126;
        significand /= (1 << 22);
    } else significand = (significand | (1 << 23)) / (1 << 23);
    return sign * significand * Math.pow(2, exponent);
  }
  function Decoder(bytes, port) {
  var lat = bytes[3] << 24 | bytes[2] << 16 | bytes[1] << 8 | bytes[0];
  var lon = bytes[7] << 24 | bytes[6] << 16 | bytes[5] << 8 | bytes[4];
  var alt = bytes[11] << 24 | bytes[10] << 16 | bytes[9] << 8 | bytes[8];
  return{
    accuracy: 2.5,
    altitude: Bytes2Float32(alt).toFixed(0),
    latitude:  Bytes2Float32(lat),
    longitude: Bytes2Float32(lon),
    

  };
  }
  ```
  ## Ajout du flow de la console helium
  
  ![image](https://user-images.githubusercontent.com/85999010/154266973-5ed3b287-c8e2-4100-8c9a-7d90387eb367.png)


Et maintenant vous n'avez plus qu'a envoyer le fichier via arduino sur la carte et l'alimenter pour tester.

::: danger Attention
Je débute dans l'arduino et mon code n'est surement pas optimisé, à vous de découvrir et vous amuser a modifier tous ce que je viens de vous partager.
:::
