# RCC STM32H723 — все регистры, все биты (включая резервные)

База `0x58024400`. Позиции сверены с `Inc/stm32h723xx.h`, семантика делителей
PLL — с кодом HAL (`DIVN/DIVP/DIVQ/DIVR` хранятся как N−1, `DIVM` — прямо).
`res` = зарезервировано. Домены: D1 (CPU+AXI), D2 (APB1/APB2), D3 (APB4).
Лимиты H723: SYSCLK 550, CPU/HCLK 275, все PCLK 137.5 МГц.

## CR (0x00) — генераторы и PLL, rw (RDY — только чтение)

| Биты | Имя | Что делает |
|---|---|---|
| 0 | HSION | Внутренний RC 64 МГц вкл (работает после сброса, стартовые такты) |
| 1 | HSIKERON | HSI не гаснет в Stop (нужен периферии, которая работает в Stop) |
| 2 | HSIRDY | ro. 1=HSI стабилен |
| 4:3 | HSIDIV[1:0] | Делитель HSI: 00=/1 (64), 01=/2 (32), 10=/4 (16), 11=/8 (8) |
| 5 | HSIDIVF | ro. 1=новый делитель HSIDIV ещё применяется |
| 6 | res | Зарезервирован |
| 7 | CSION | Внутренний RC ~4 МГц вкл |
| 8 | CSIRDY | ro. 1=CSI стабилен |
| 9 | CSIKERON | CSI не гаснет в Stop |
| 11:10 | res | Зарезервированы |
| 12 | HSI48ON | RC 48 МГц вкл (нужен USB/RNG как вариант тактов) |
| 13 | HSI48RDY | ro. 1=HSI48 стабилен |
| 14 | D1CKRDY | ro. 1=домен D1 тактируется (контроль выхода из Stop) |
| 15 | D2CKRDY | ro. 1=домен D2 тактируется |
| 16 | HSEON | Кварц вкл |
| 17 | HSERDY | ro. 1=кварц стабилен. Нет флага за разумное время = кварца нет/сдох |
| 18 | HSEBYP | 1=вместо кварца внешний меандр на HSE_IN (bypass) |
| 19 | CSSHSEON | Clock Security на HSE: при смерти кварца — NMI + автопереход на HSI |
| 23:20 | res | Зарезервированы |
| 24 | PLL1ON (=PLLON) | PLL1 вкл |
| 25 | PLL1RDY (=PLLRDY) | ro. PLL1 залочена |
| 26 | PLL2ON | PLL2 вкл |
| 27 | PLL2RDY | ro. PLL2 залочена |
| 28 | PLL3ON | PLL3 вкл |
| 29 | PLL3RDY | ro. PLL3 залочена |
| 31:30 | res | Зарезервированы |

Порядок: вкл генератор → ждать RDY → и только потом использовать его
как источник PLL/SYSCLK. Выключать неиспользуемое — экономия тока.

## HSICFGR (0x04) — калибровка HSI, rw

| Биты | Имя | Что делает |
|---|---|---|
| 11:0 | HSICAL[11:0] | Заводская калибровка HSI (не трогать без нужды) |
| 23:12 | res | Зарезервированы |
| 30:24 | HSITRIM[6:0] | Подстройка частоты HSI пользователем |
| 31 | res | Зарезервирован |

## CRRCR (0x08) — калибровка HSI48, ro

| Биты | Имя | Что делает |
|---|---|---|
| 9:0 | HSI48CAL[9:0] | Заводская калибровка HSI48 |
| 31:10 | res | Зарезервированы |

## CFGR (0x10) — выбор SYSCLK, MCO, RTC, таймеры, rw

| Биты | Имя | Что делает |
|---|---|---|
| 2:0 | SW[2:0] | Источник SYSCLK: 000=HSI (сброс), 001=CSI, 010=HSE, 011=PLL1 |
| 5:3 | SWS[2:0] | ro. Текущий источник (кодировка та же). После записи SW ждать SWS==SW |
| 6 | STOPWUCK | Чем просыпаться из Stop: 0=HSI, 1=CSI |
| 7 | STOPKERWUCK | Ядро периферии в Stop: 0=HSI, 1=CSI |
| 13:8 | RTCPRE[5:0] | Делитель HSE для RTC (0=выкл, иначе деление на значение) |
| 14 | res | Зарезервирован |
| 15 | TIMPRE | Такты таймеров: 0=как шина (PCLK при /1, иначе 2×PCLK); 1=удвоенные и при /1,/2,/4 (до ×4) |
| 17:16 | res | Зарезервированы |
| 21:18 | MCO1PRE[3:0] | Делитель MCO1: значение = деление (1–15) |
| 24:22 | MCO1[2:0] | Источник MCO1 (PA8): 000=HSI, 001=LSE, 010=HSE, 011=PLL1_Q, 100=HSI48 |
| 28:25 | MCO2PRE[3:0] | Делитель MCO2: значение = деление (1–15) |
| 31:29 | MCO2[2:0] | Источник MCO2 (PC9): 000=SYSCLK, 001=PLL2_P, 010=HSE, 011=PLL1_P, 100=CSI, 101=LSI |

MCO-пин: GPIO в AF, у MCO1/MCO2 это AF0. Наше эхо: MCO1=PLL1_Q, /15 →
1.145833 МГц на PA8.

## D1CFGR (0x18), D2CFGR (0x1C), D3CFGR (0x20) — делители шин, rw

| Биты | Регистр.поле | Делит | Значения |
|---|---|---|---|
| 3:0 | D1CFGR.HPRE[3:0] | SYSCLK→HCLK(AHB) | 0xxx=/1, 1000=/2, 1001=/4, 1010=/8, 1011=/16, 1100=/64, 1101=/128, 1110=/256, 1111=/512 |
| 6:4 | D1CFGR.D1PPRE[2:0] | HCLK→APB3 | 0xx=/1, 100=/2, 101=/4, 110=/8, 111=/16 |
| 7 | D1CFGR.res | — | Зарезервирован |
| 11:8 | D1CFGR.D1CPRE[3:0] | SYSCLK→CPU | та же шкала, что HPRE |
| 31:12 | D1CFGR.res | — | Зарезервированы |
| 3:0 | D2CFGR.res | — | Зарезервированы |
| 6:4 | D2CFGR.D2PPRE1[2:0] | HCLK→APB1 | шкала /1–/16 как выше |
| 7 | D2CFGR.res | — | Зарезервирован |
| 10:8 | D2CFGR.D2PPRE2[2:0] | HCLK→APB2 | шкала /1–/16 как выше |
| 31:11 | D2CFGR.res | — | Зарезервированы |
| 3:0 | D3CFGR.res | — | Зарезервированы |
| 6:4 | D3CFGR.D3PPRE[2:0] | HCLK→APB4 | шкала /1–/16 как выше |
| 31:7 | D3CFGR.res | — | Зарезервированы |

Важно: ядро CPU сидит на D1CPRE (по сбросу /1), а не напрямую на SYSCLK.
Таймеры при предделителе APB>1 тактируются удвоенной частотой шины.

## PLLCKSELR (0x28) — входы PLL, rw (когда PLL выкл)

| Биты | Имя | Что делает |
|---|---|---|
| 1:0 | PLLSRC[1:0] | Общий источник всех PLL: 00=HSI, 01=CSI, 10=HSE, 11=выкл |
| 3:2 | res | Зарезервированы |
| 9:4 | DIVM1[5:0] | Делитель опоры PLL1: значение = деление (/1–/63, 0=/1) |
| 15:10 | DIVM2[5:0] | То же для PLL2 |
| 21:16 | DIVM3[5:0] | То же для PLL3 |
| 31:22 | res | Зарезервированы |

Опора после DIVM обязана попасть в диапазон RGE (см. PLLCFGR):
1–2, 2–4, 4–8 или 8–16 МГц. Наше эхо: HSE + DIVM1=5 → 5 МГц.

## PLLCFGR (0x2C) — режимы PLL, rw (когда PLL выкл)

| Биты | Имя | Что делает |
|---|---|---|
| 0 | PLL1FRACEN | Дробный режим PLL1 (N + FRACN/8192). Без него — целый N |
| 1 | PLL1VCOSEL | Диапазон VCO1: 0=широкий 128–560 МГц, 1=средний 150–420 |
| 3:2 | PLL1RGE[1:0] | Диапазон опоры PLL1: 00=1–2, 01=2–4, 10=4–8, 11=8–16 МГц. Должен соответствовать реальной опоре, иначе джиттер/нелочка |
| 4 | PLL2FRACEN | То же для PLL2 |
| 5 | PLL2VCOSEL | Диапазон VCO2 |
| 7:6 | PLL2RGE[1:0] | Диапазон опоры PLL2 |
| 8 | PLL3FRACEN | То же для PLL3 |
| 9 | PLL3VCOSEL | Диапазон VCO3 |
| 11:10 | PLL3RGE[1:0] | Диапазон опоры PLL3 |
| 15:12 | res | Зарезервированы |
| 16/17/18 | DIVP1EN/DIVQ1EN/DIVR1EN | Выходы P/Q/R PLL1 вкл (P — системный клок) |
| 19/20/21 | DIVP2EN/DIVQ2EN/DIVR2EN | Выходы PLL2 вкл |
| 22/23/24 | DIVP3EN/DIVQ3EN/DIVR3EN | Выходы PLL3 вкл |
| 31:25 | res | Зарезервированы |

Наше эхо: VCOSEL=0, RGE=10, DIVP1EN=1, DIVQ1EN=1.

## PLLnDIVR + PLLnFRACR — коэффициенты, rw (когда PLL выкл)

PLL1DIVR 0x30, PLL1FRACR 0x34, PLL2DIVR 0x38, PLL2FRACR 0x3C,
PLL3DIVR 0x40, PLL3FRACR 0x44. Поля одинаковые:

| Биты (в DIVR) | Поле | Смысл (подтверждено кодом HAL) |
|---|---|---|
| 8:0 | DIVN[8:0] | Умножитель: регистр = N−1 (4–512). У нас 109 → ×110 |
| 15:9 | DIVP[6:0] | Делитель P: регистр = P−1 (0 → /1). У нас 0 |
| 22:16 | DIVQ[6:0] | Делитель Q: регистр = Q−1. У нас 31 → /32 |
| 30:24 | DIVR[6:0] | Делитель R: регистр = R−1 |
| 31 | res | Зарезервирован |

FRACR: `FRACN[12:0]` (биты 12:0, остальные res) — дробь N:
VCO = опора × (N + FRACN/8192), только с FRACEN.
VCO обязан попасть в выбранный VCOSEL диапазон. У нас: 5×110=550 ≤ 560.

## D1CCIPR (0x4C) — такты D1-периферии, rw

| Биты | Имя | Что делает |
|---|---|---|
| 1:0 | FMCSEL[1:0] | Ядро FMC: 00=HCLK, 01=PLL1_Q, 10=PLL2_R, 11=PER_CK |
| 3:2 | res | Зарезервированы |
| 5:4 | OCTOSPISEL[1:0] | Ядро OCTOSPI: 00=HCLK, 01=PLL1_Q, 10=PLL2_R, 11=PER_CK |
| 15:6 | res | Зарезервированы |
| 16 | SDMMCSEL | Ядро SDMMC1: 0=PLL1_Q, 1=PLL2_R |
| 27:17 | res | Зарезервированы |
| 29:28 | CKPERSEL[1:0] | Общий PER_CK: 00=HSI, 01=CSI, 10=HSE |
| 31:30 | res | Зарезервированы |

## D2CCIP1R (0x50) — такты D2-периферии 1, rw

| Биты | Имя | Что делает |
|---|---|---|
| 2:0 | SAI1SEL[2:0] | SAI1: 000=PLL1_Q, 001=PLL2_P, 010=PLL3_P, 011=I2S_CKIN, 100=PER_CK |
| 11:3 | res | Зарезервированы |
| 14:12 | SPI123SEL[2:0] | SPI1/2/3: та же шкала, что SAI1SEL |
| 15 | res | Зарезервирован |
| 18:16 | SPI45SEL[2:0] | SPI4/5: та же шкала |
| 19 | res | Зарезервирован |
| 21:20 | SPDIFSEL[1:0] | SPDIF: 00=PLL1_Q, 01=PLL2_Q, 10=HSI |
| 23:22 | res | Зарезервированы |
| 24 | DFSDM1SEL | DFSDM1: 0=PCLK, 1=SYSCLK |
| 27:25 | res | Зарезервированы |
| 29:28 | FDCANSEL[1:0] | FDCAN: 00=HSE, 01=PLL1_Q, 10=PLL2_Q |
| 30 | res | Зарезервирован |
| 31 | SWPSEL | SWPMI: 0=PCLK, 1=HSI |

## D2CCIP2R (0x54) — такты D2-периферии 2, rw

| Биты | Имя | Что делает |
|---|---|---|
| 2:0 | USART28SEL[2:0] | USART2/8: 000=PCLK, 001=PLL2_Q, 010=PLL3_Q, 011=HSI, 100=CSI, 101=LSE |
| 5:3 | USART16910SEL[2:0] | USART1/6/9/10: та же шкала (у нас сброс 000=PCLK2) |
| 7:6 | res | Зарезервированы |
| 9:8 | RNGSEL[1:0] | RNG: 00=HSI48, 01=PLL1_Q, 10=PLL2_Q (точные коды — RM0468) |
| 11:10 | res | Зарезервированы |
| 13:12 | I2C1235SEL[1:0] | I2C1/2/3/5: 00=PCLK, 01=PLL3_R, 10=HSI, 11=CSI |
| 19:14 | res | Зарезервированы |
| 21:20 | USBSEL[1:0] | USB: 00=выкл, 01=PLL1_Q, 10=PLL3_Q, 11=HSI48 |
| 23:22 | CECSEL[1:0] | CEC: 00=LSE, 01=LSI, 10=CSI (точные коды — RM0468) |
| 27:24 | res | Зарезервированы |
| 30:28 | LPTIM1SEL[2:0] | LPTIM1: 000=PCLK, 001=PLL2_P, 010=PLL3_P, 011=LSE, 100=LSI, 101=PER_CK |
| 31 | res | Зарезервирован |

Нюанс H723: у USART3/UART4/UART5/UART7 мультиплексора НЕТ — ядро всегда PCLK1.

## D3CCIPR (0x58) — такты D3-периферии, rw

| Биты | Имя | Что делает |
|---|---|---|
| 2:0 | LPUART1SEL[2:0] | LPUART1: 000=PCLK4, 001=PLL2_Q, 010=PLL3_Q, 011=HSI, 100=CSI, 101=LSE |
| 7:3 | res | Зарезервированы |
| 9:8 | I2C4SEL[1:0] | I2C4: 00=PCLK4, 01=PLL3_R, 10=HSI, 11=CSI |
| 12:10 | LPTIM2SEL[2:0] | LPTIM2: шкала как LPTIM1SEL |
| 15:13 | LPTIM345SEL[2:0] | LPTIM3/4/5: та же шкала |
| 17:16 | ADCSEL[1:0] | ADC3: 00=PLL2_P, 01=PLL3_P, 10=HSI (точные коды — RM0468) |
| 20:18 | res | Зарезервированы |
| 23:21 | SAI4ASEL[2:0] | SAI4_A: шкала как SAI1SEL |
| 26:24 | SAI4BSEL[2:0] | SAI4_B: та же шкала |
| 27 | res | Зарезервирован |
| 30:28 | SPI6SEL[2:0] | SPI6: шкала как SPI123SEL |
| 31 | res | Зарезервирован |

## CIER (0x60, rw) / CIFR (0x64, r) / CICR (0x68, w1c) — прерывания готовности

| Бит | CIER (разрешить) | CIFR (флаг) | CICR (сброс записью 1) |
|---|---|---|---|
| 0 | LSIRDYIE | LSIRDYF | LSIRDYC |
| 1 | LSERDYIE | LSERDYF | LSERDYC |
| 2 | HSIRDYIE | HSIRDYF | HSIRDYC |
| 3 | HSERDYIE | HSERDYF | HSERDYC |
| 4 | CSIRDYIE | CSIRDYF | CSIRDYC |
| 5 | HSI48RDYIE | HSI48RDYF | HSI48RDYC |
| 6 | PLL1RDYIE | PLLRDYF (это PLL1) | PLLRDYC |
| 7 | PLL2RDYIE | PLL2RDYF | PLL2RDYC |
| 8 | PLL3RDYIE | PLL3RDYF | PLL3RDYC |
| 9 | LSECSSIE | LSECSSF | LSECSSC |
| 10 | res | HSECSSF (CSS сработал на HSE) | HSECSSC |
| 31:11 | res (кроме CIFR/CICR бит 10) | — | — |

Классика: разрешил → ждёшь флаг → гасишь.

## BDCR (0x70) — backup-домен, rw (нужен PWR_CR1.DBP=1!)

| Биты | Имя | Доступ | Что делает |
|---|---|---|---|
| 0 | LSEON | rw | Кварц 32.768 кГц вкл |
| 1 | LSERDY | ro | 1=стабилен |
| 2 | LSEBYP | rw | 1=внешний меандр вместо кварца |
| 4:3 | LSEDRV[1:0] | rw | Мощность драйвера кварца: 00=слабо … 11=максимум (под ёмкость кварца) |
| 5 | LSECSSON | rw | CSS на LSE |
| 6 | LSECSSD | ro | 1=CSS поймал смерть LSE |
| 7 | res | — | Зарезервирован |
| 9:8 | RTCSEL[1:0] | rw | Такты RTC: 00=нет, 01=LSE, 10=LSI, 11=HSE(/RTCPRE) |
| 14:10 | res | — | Зарезервированы |
| 15 | RTCEN | rw | RTC включён |
| 16 | VSWRST | rw | Сброс VSW-домена |
| 17 | BDRST | rw | 1=сброс всего backup-домена (RTC+бакрегистры+BDCR). Самоочищается |
| 31:18 | res | — | Зарезервированы |

## CSR (0x74) — LSI

| Биты | Имя | Что делает |
|---|---|---|
| 0 | LSION | RC ~32 кГц вкл (сторожит IWDG/RTC) |
| 1 | LSIRDY | ro. 1=стабилен |
| 31:2 | res | Зарезервированы |

## RSR (0xD0) — причины сброса, r (сброс — битом RMVF)

| Биты | Имя | Что делает |
|---|---|---|
| 15:0 | res | Зарезервированы |
| 16 | RMVF | rw. Запись 1 сбрасывает все флаги ниже |
| 17 | CPURSTF | Сброс CPU-домена |
| 18 | res | Зарезервирован |
| 19 | D1RSTF | Сброс домена D1 |
| 20 | D2RSTF | Сброс домена D2 |
| 21 | BORRSTF | Brown-out reset |
| 22 | PINRSTF | Ножка NRST |
| 23 | PORRSTF | Включение питания |
| 24 | SFTRSTF | Программный (через NVIC SYSRESETREQ) |
| 25 | res | Зарезервирован |
| 26 | IWDG1RSTF | Независимый watchdog |
| 27 | res | Зарезервирован |
| 28 | WWDG1RSTF | Оконный watchdog |
| 29 | res | Зарезервирован |
| 30 | LPWRRSTF | Выход из LP-режимов |
| 31 | res | Зарезервирован |

Читать причину в самом начале старта, потом чистить через RMVF.

## GCR (0xA0)

| Биты | Имя | Что делает |
|---|---|---|
| 0 | WW1RSC | Область действия сброса WWDG1 (детали — RM0468) |
| 31:1 | res | Зарезервированы |

## RSTR — сброс периферии, rw (1=держать в сбросе, потом вернуть 0)

AHB3RSTR 0x7C: MDMA(0), DMA2D(4), FMC(12), OSPI1(14), SDMMC1(16), OSPI2(19),
IOMNGR(21), CPURST(31 — сброс CPU-домена!).
AHB1RSTR 0x80: DMA1(0), DMA2(1), ADC12(5), ETH1MAC(15), USB1OTGHS(25).
AHB2RSTR 0x84: DCMI_PSSI(0, дубль DCMIRST), RNG(6), SDMMC2(9), FMAC(16), CORDIC(17).
AHB4RSTR 0x88: GPIOA–K(0–7,9,10; нет GPIOI), CRC(19), BDMA(21), ADC3(24), HSEM(25).
APB3RSTR 0x8C: LTDC(3).
APB1LRSTR 0x90: TIM2–7(0–5), TIM12/13/14(6,7,8), LPTIM1(9), SPI2(14), SPI3(15),
SPDIFRX(16), USART2(17), USART3(18), UART4(19), UART5(20), I2C1(21), I2C2(22),
I2C3(23), I2C5(25), CEC(27, дубль HDMICEC), DAC12(29), UART7(30), UART8(31).
APB1HRSTR 0x94: CRS(1), SWPMI(2), OPAMP(4), MDIOS(5), FDCAN(8), TIM23(24), TIM24(25).
APB2RSTR 0x98: TIM1(0), TIM8(1), USART1(4), USART6(5), UART9(6), USART10(7),
SPI1(12), SPI4(13), TIM15(16), TIM16(17), TIM17(18), SPI5(20), SAI1(22), DFSDM1(30).
APB4RSTR 0x9C: SYSCFG(1), LPUART1(3), SPI6(5), I2C4(7), LPTIM2(9), LPTIM3(10),
LPTIM4(11), LPTIM5(12), COMP12(14), VREF(15), SAI4(21), DTS(26).
Остальные биты в каждом — res. Типовой сброс периферии: ENR=1 →
RSTR=1 → RSTR=0 (короткий импульс через 2 холостых чтения).

## ENR — такты периферии, rw (1=тактовать). Без бита регистры молчат

AHB3ENR 0xD4: MDMA(0), DMA2D(4), FMC(12), OSPI1(14), SDMMC1(16), OSPI2(19), IOMNGR(21).
AHB1ENR 0xD8: DMA1(0), DMA2(1), ADC12(5), ETH1MAC(15), ETH1TX(16), ETH1RX(17),
USB1OTGHS(25), USB1OTGHSULPI(26).
AHB2ENR 0xDC: DCMI_PSSI(0), RNG(6), SDMMC2(9), FMAC(16), CORDIC(17),
SRAM1(29), SRAM2(30).
AHB4ENR 0xE0: GPIOA–K(0–7,9,10), CRC(19), BDMA(21), ADC3(24), HSEM(25), BKPRAM(28).
APB3ENR 0xE4: LTDC(3), WWDG1(6).
APB1LENR 0xE8: TIM2–7(0–5), TIM12/13/14(6,7,8), LPTIM1(9), SPI2(14), SPI3(15),
SPDIFRX(16), USART2(17), USART3(18), UART4(19), UART5(20), I2C1(21), I2C2(22),
I2C3(23), I2C5(25), CEC(27, дубль HDMICEC), DAC12(29), UART7(30), UART8(31).
APB1HENR 0xEC: CRS(1), SWPMI(2), OPAMP(4), MDIOS(5), FDCAN(8), TIM23(24), TIM24(25).
APB2ENR 0xF0: TIM1(0), TIM8(1), USART1(4), USART6(5), UART9(6), USART10(7),
SPI1(12), SPI4(13), TIM15(16), TIM16(17), TIM17(18), SPI5(20), SAI1(22), DFSDM1(30).
APB4ENR 0xF4: SYSCFG(1), LPUART1(3), SPI6(5), I2C4(7), LPTIM2(9), LPTIM3(10),
LPTIM4(11), LPTIM5(12), COMP12(14), VREF(15), RTCAPB(16), SAI4(21), DTS(26).
Остальные биты в каждом — res.
После установки бита — холостое чтение регистра (такт доходит за 1–2 цикла).
На H723 бита тактов PWR нет — PWR тактируется всегда.

## LPENR — такты в Sleep, rw (по сбросу всё включено)

Карта бит повторяет ENR (те же номера, суффикс LPEN), плюс биты памятей:
AHB3LPENR 0xFC: +FLASH(8), DTCM1(28), DTCM2(29), ITCM(30), AXISRAM(31).
AHB1LPENR 0x100: зеркало ENR (ETH1TXLPEN 16, ETH1RXLPEN 17 тоже есть).
AHB2LPENR 0x104: зеркало ENR.
AHB4LPENR 0x108: GPIOA–K(0–7,9,10), CRC(19), BDMA(21), ADC3(24), BKPRAM(28),
SRAM4(29, дубль D3SRAM1LPEN). Бита HSEM здесь нет (только в ENR/RSTR).
APB3LPENR 0x10C, APB1LLPENR 0x110, APB1HLPENR 0x114, APB2LPENR 0x118,
APB4LPENR 0x11C: зеркала ENR (HDMICECLPEN = CECLPEN, бит 27).
Смысл: 0=в Sleep такты периферии/памяти гаснут (экономия), 1=остаются.
