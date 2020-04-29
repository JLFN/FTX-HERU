Här kommer källkoden till de programmen jag använder för att kommunicera med mitt Heru aggregat. Det är bland det värsta och rörigaste jag någonsin skrivit och är inte alls stolt över den, p.g.a tidsbrist så har jag inte hunnit städa upp
Stora delar är klippt och klistrade från andra program så endel av funktionerna används inte. Finns även rent felaktiga kommentarer i koden :)
Två filer finns bifogade den ena har jag använt för att logga trafiken mellan Fjärren och aggregatet, den andra är för att kommunicera med aggregatet via modbus tcp. För att förenkla det hela har jag helt enkelt kollat vilka kommandon fjärren sänder och sen sänder jag samma sak med arduinon, då behöver man inte tänka på crc korrigering. Svaret från aggregatet kodas sen upp och kan läsas via modbus. Dethär gör att programmet inte kommer att funka på något annat aggregat än mitt utan ändringar eftersom fjärrens och aggregatets id hela tiden skickas med i paketen som är hårdkodade i programmet. Ni som kan lite om programmering kan nog förstå hur det funkar, fråga annars så ska jag försöka förklara :)

Jag kör dessa på en Wemos D1 R2(så egentligen inte en arduino) som är kopplad till en NRF905
Dessa kinatillverkade NRF905:or verkar vara designade för en annan frekvens än den som används så räckvidden blir tyvärr begränsad till några meter.

Notera Fjärrskontrollen måste ha Modbus funktionen annars ska det går att programmera om EEPROM på kretskortet från fjärrkontrollen.
``
Esp8266 D1 Mini--nrf905
D0----------------DR
D1----------------CE
D2----------------TXEN
D3----------------PWR
3V3---------------VCC
D5----------------SCK
D6----------------MISO
D7----------------MOSI
D8----------------CSN
GND---------------GND

``
