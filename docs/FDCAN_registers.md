# FDCAN — справочник регистров (STM32H723VGT6, CMSIS, без HAL, ядро Bosch M_CAN)

Источник позиций/ширин бит: `Inc/stm32h723xx.h` (проверено grep по `FDCAN_CREL/ENDN/DBTP/TEST/RWD/CCCR/NBTP/TSCC/TSCV/TOCC/TOCV/ECR/PSR/TDCR/IR/IE/ILS/ILE/GFC/SIDFC/XIDFC/XIDAM/HPMS/NDAT/RXF0C/RXF0S/RXF0A/RXBC/RXF1C/RXF1S/RXF1A/RXESC/TXBC/TXFQS/TXESC/TXBRP/TXBAR/TXBCR/TXBTO/TXBCF/TXBTIE/TXBCIE/TXEFC/TXEFS/TXEFA_*_Pos` + структ `FDCAN_GlobalTypeDef` для смещений).
Формат: заголовок, таблицы `| Биты | Имя | Что делает |`, строки `res`, русский язык.

Базовые адреса (из заголовка):

| Модуль | База |
|---|---|
| FDCAN1 | 0x4000A000 (`D2_APB1 + 0xA000`) |
| FDCAN2 | 0x4000A400 (`D2_APB1 + 0xA400`) |
| FDCAN3 | 0x4000D400 (`D2_APB1 + 0xD400`) |
| Message RAM (общая, SRAMCAN) | 0x4000AC00 (`D2_APB1 + 0xAC00`) |

Смещения (`FDCAN_GlobalTypeDef`): CREL 0x000, ENDN 0x004, DBTP 0x00C, TEST 0x010, RWD 0x014, CCCR 0x018, NBTP 0x01C, TSCC 0x020, TSCV 0x024, TOCC 0x028, TOCV 0x02C, ECR 0x040, PSR 0x044, TDCR 0x048, IR 0x050, IE 0x054, ILS 0x058, ILE 0x05C, GFC 0x080, SIDFC 0x084, XIDFC 0x088, XIDAM 0x090, HPMS 0x094, NDAT1 0x098, NDAT2 0x09C, RXF0C 0x0A0, RXF0S 0x0A4, RXF0A 0x0A8, RXBC 0x0AC, RXF1C 0x0B0, RXF1S 0x0B4, RXF1A 0x0B8, RXESC 0x0BC, TXBC 0x0C0, TXFQS 0x0C4, TXESC 0x0C8, TXBRP 0x0CC, TXBAR 0x0D0, TXBCR 0x0D4, TXBTO 0x0D8, TXBCF 0x0DC, TXBTIE 0x0E0, TXBCIE 0x0E4, TXEFC 0x0F0, TXEFS 0x0F4, TXEFA 0x0F8. Резерв: 0x008, 0x030–0x03C, 0x04C, 0x060–0x07C, 0x08C, 0x0E8–0x0EC, 0x0FC.

Уточнения по именам из задачи (расхождений с заголовком несколько, ниже — имена из заголовка):
- `RXGFC` → в заголовке `GFC` (Global Filter Configuration); `TOEC` → `TOCV` (Timeout Counter Value); `XIDAM`/`HPMS` — как в задаче; `TXBAR?` второй раз — на деле пара `TXBAR` (add request) / `TXBCR` (cancel) / `TXBTO` (occurred) / `TXBCF` (cancel finished).

## CREL (0x000) / ENDN (0x004) — идентификация

CREL (только чтение):

| Биты | Имя | Что делает |
|---|---|---|
| 7:0 | DAY | День релиза ядра |
| 15:8 | MON | Месяц релиза ядра |
| 19:16 | YEAR | Год релиза ядра |
| 23:20 | SUBSTEP | Подшаг релиза |
| 27:24 | STEP | Шаг релиза |
| 31:28 | REL | Релиз ядра |

ENDN:

| Биты | Имя | Что делает |
|---|---|---|
| 31:0 | ETV | Тест порядка байт, всегда `0x87654321`. Чтение — проверка endianness |

## DBTP (0x00C) — data bit timing (фаза данных, FD). Менять при INIT+CCE

| Биты | Имя | Что делает |
|---|---|---|
| 3:0 | DSJW | Data (re)synchronization jump width минус 1: `0–15` → `1–16 TQ` |
| 7:4 | DTSEG2 | Data time segment 2 минус 1: `0–15` → `1–16 TQ` |
| 12:8 | DTSEG1 | Data time segment 1 минус 1 (+ SyncSeg уже включён): `0–31` → `1–32 TQ` |
| 15:13 | `res` | Зарезервированы |
| 20:16 | DBRP | Data baud rate prescaler минус 1: `0–31` → делитель `1–32` |
| 22:21 | `res` | Зарезервированы |
| 23 | TDC | 1 = transmitter delay compensation включён (обязателен при высоких скоростях данных, см. TDCR) |
| 31:24 | `res` | Зарезервированы |

## TEST (0x010) / RWD (0x014)

TEST (писать при INIT+CCE; `CCCR.TEST=1` открывает):

| Биты | Имя | Что делает |
|---|---|---|
| 3:0 | `res` | Зарезервированы |
| 4 | LBCK | 1 = Loop Back (внутренняя петля для самотеста) |
| 6:5 | TX | Управление пином TX: `0=по протоколу, 1=доминанта, 2=рецессив, 3=res` |
| 7 | RX | Текущий уровень пина RX (чтение) |
| 31:8 | `res` | Зарезервированы |

RWD (RAM watchdog):

| Биты | Имя | Что делает |
|---|---|---|
| 7:0 | WDC | Значение watchdog для доступа к Message RAM (таймаут в тактах) |
| 15:8 | WDV | Текущее значение счётчика (чтение) |
| 31:16 | `res` | Зарезервированы |

## CCCR (0x018) — главный контроль. Вход в конфигурацию: INIT=1 + CCE=1

| Биты | Имя | Что делает |
|---|---|---|
| 0 | INIT | 1 = инициализация (останов протокола). Вход: `INIT=1`, ждать `CCCR` применения; выход: `INIT=0` |
| 1 | CCE | 1 = разрешена смена конфигурации (только когда `INIT=1`). Связка для любых настроек: `INIT+CCE=1` |
| 2 | ASM | 1 = restricted operation (слушать, но не отвечать ACK/активные флаги ошибок) |
| 3 | CSA | 1 = clock stop acknowledge (запрос останова тактов подтверждён) |
| 4 | CSR | Запрос останова тактов (clock stop request) |
| 5 | MON | 1 = bus monitoring (только слушать, без передачи) |
| 6 | DAR | 1 = запретить авто-ретрай (disable automatic retransmission) |
| 7 | TEST | 1 = тестовый режим (открывает `TEST` регистр) |
| 8 | FDOE | 1 = разрешён FD (длинные кадры до 64 байт + BRS), 0 = только классический CAN 2.0 |
| 9 | BRSE | 1 = разрешён bit rate switch (переход на скорость данных в FD) |
| 11:10 | `res` | Зарезервированы |
| 12 | PXHD | 1 = запретить protocol exception handling (жёсткая ошибка на зарезервированные биты) |
| 13 | EFBI | 1 = edge filtering при интеграции в шину (фильтр дребезга) |
| 14 | TXP | 1 = разрешить паузу между передачами (transmit pause, минимум 2 бита простоя) |
| 15 | NISO | 1 = non-ISO FD (старый Bosch FD), 0 = ISO 11898-1 FD |
| 31:16 | `res` | Зарезервированы |

## NBTP (0x01C) — nominal bit timing (арбитраж/классика). Менять при INIT+CCE

| Биты | Имя | Что делает |
|---|---|---|
| 6:0 | NTSEG2 | Nominal time segment 2 минус 1: `0–127` → `1–128 TQ` |
| 7 | `res` | Зарезервирован |
| 15:8 | NTSEG1 | Nominal time segment 1 минус 1 (+ SyncSeg): `0–255` → `1–256 TQ` |
| 24:16 | NBRP | Nominal baud rate prescaler минус 1: `0–511` → делитель `1–512` |
| 31:25 | NSJW | Nominal (re)synchronization jump width минус 1: `0–127` → `1–128 TQ` |

Формулы битрейта (обе фазы одинаково): `TQ = (BRP+1) / f_can_ker`; `бит = 1 + TSEG1+1 + TSEG2+1` TQ; `baud = f_can_ker / ((BRP+1) * (1 + (TSEG1+1) + (TSEG2+1)))`; `SJW ≤ min(TSEG1,TSEG2)`, семпл-поинт ≈ `(1+TSEG1+1)/бит` (цель 75–87.5%).

## TSCC (0x020) / TSCV (0x024) — timestamp

TSCC:

| Биты | Имя | Что делает |
|---|---|---|
| 1:0 | TSS | Источник метки: `0=выкл, 1=счётчик +1 каждый CAN-бит, 2=внешний (по `TCP`), 3=res` |
| 15:2 | `res` | Зарезервированы |
| 19:16 | TCP | Предделитель внешнего источника (`0–15` → `/1–/16`) |
| 31:20 | `res` | Зарезервированы |

TSCV: `TSC[15:0]` — текущее значение счётчика (младшие 16 бит), `31:16 res`.

## TOCC (0x028) / TOCV (0x02C) — timeout

TOCC:

| Биты | Имя | Что делает |
|---|---|---|
| 0 | ETOC | 1 = включить timeout-счётчик |
| 2:1 | TOS | Что считать: `0=непрерывный (от TSC), 1=timeout по TX Event FIFO, 2=timeout по RX FIFO 0, 3=по RX FIFO 1` |
| 15:3 | `res` | Зарезервированы |
| 31:16 | TOP | Предзагрузка/период timeout (`0–65535`) |

TOCV: `TOC[15:0]` — текущее значение timeout-счётчика, `31:16 res`. Флаг срабатывания — `IR.TOO`.

## ECR (0x040) — счётчики ошибок

| Биты | Имя | Что делает |
|---|---|---|
| 7:0 | TEC | Transmit error counter (`0–255`) |
| 14:8 | REC | Receive error counter (`0–127`) |
| 15 | RP | 1 = узел в recessive (error passive), 0 = active |
| 23:16 | CEL | Counter error logging (счётчик инкрементов ошибок) |
| 31:24 | `res` | Зарезервированы |

## PSR (0x044) — статус протокола (чтение)

| Биты | Имя | Что делает |
|---|---|---|
| 2:0 | LEC | Last error code: `0=нет, 1=stuff, 2=form, 3=ack, 4=bit1, 5=bit0, 6=CRC, 7=без изменений` |
| 4:3 | ACT | Активность: `0=синхронизация, 1=простой, 2=приём, 3=передача` |
| 5 | EP | 1 = error passive (TEC>127 или REC>127) |
| 6 | EW | 1 = warning (TEC/REC ≥ 96) |
| 7 | BO | 1 = bus off (TEC>255) |
| 10:8 | DLEC | Data-phase last error code (та же кодировка, что LEC, но для фазы данных FD) |
| 11 | RESI | Последний принятый ESI-бит (error state indicator) |
| 12 | RBRS | Последний принятый BRS-бит (bit rate switch) |
| 13 | REDL | Последний принятый EDL-бит (extended data length, 1=FD-кадр) |
| 14 | PXE | Protocol exception event (при `PXHD=0` — защёлкнут факт исключения) |
| 15 | `res` | Зарезервирован |
| 22:16 | TDCV | Значение transmitter delay compensation (измеренная задержка, при `DBTP.TDC=1`) |
| 31:23 | `res` | Зарезервированы |

## TDCR (0x048) — компенсация задержки передатчика (писать при INIT+CCE)

| Биты | Имя | Что делает |
|---|---|---|
| 6:0 | TDCF | Фильтр: окно измерения вторичной выборки (`0–127` TQ) |
| 7 | `res` | Зарезервирован |
| 14:8 | TDCO | Офсет компенсации (`0–127` TQ, обычно = `DTSEG1+2`) |
| 31:15 | `res` | Зарезервированы |

## IR (0x050) / IE (0x054) / ILS (0x058) / ILE (0x05C) — прерывания

IR: флаги, сброс записью 1. IE: разрешения (имена = IR + `E`, напр. `RF0NE` vs `RF0N`). ILS: выбор линии (`0=INT0, 1=INT1`; имена = IR + `L`, напр. `RF0NL`). ILE: вкл линий `EINT0` (бит 0), `EINT1` (бит 1).

| Бит IR / IE / ILS | IR-имя | IE-имя | Что делает |
|---|---|---|---|
| 0 | RF0N | RF0NE | FIFO0: новое сообщение |
| 1 | RF0W | RF0WE | FIFO0: достиг watermark |
| 2 | RF0F | RF0FE | FIFO0: полон |
| 3 | RF0L | RF0LE | FIFO0: потеряно (переполнение) |
| 4 | RF1N | RF1NE | FIFO1: новое сообщение |
| 5 | RF1W | RF1WE | FIFO1: watermark |
| 6 | RF1F | RF1FE | FIFO1: полон |
| 7 | RF1L | RF1LE | FIFO1: потеряно |
| 8 | HPM | HPME | Принято high-priority сообщение (см. HPMS) |
| 9 | TC | TCE | Передача завершена (успешно) |
| 10 | TCF | TCFE | Отмена передачи завершена |
| 11 | TFE | TFEE | TX FIFO пуст |
| 12 | TEFN | TEFNE | TX Event FIFO: новая запись |
| 13 | TEFW | TEFWE | TX Event FIFO: watermark |
| 14 | TEFF | TEFFE | TX Event FIFO: полон |
| 15 | TEFL | TEFLE | TX Event FIFO: потеряно |
| 16 | TSW | TSWE | Timestamp wrap (переполнение TSC) |
| 17 | MRAF | MRAFE | Ошибка доступа к Message RAM (см. RWD) |
| 18 | TOO | TOOE | Timeout (см. TOCC/TOCV) |
| 19 | DRX | DRXE | Сообщение сохранено в dedicated RX buffer (см. NDAT) |
| 20 | (только IE: BECE) | BECE | (IR бит 20 res) Разреш. прерывания по bit-error-corrected |
| 21 | (только IE: BEUE) | BEUE | (IR бит 21 res) Разреш. по bit-error-uncorrected |
| 22 | ELO | ELOE | Error logging overflow (переполнение CEL) |
| 23 | EP | EPE | Переход в error passive |
| 24 | EW | EWE | Переход в warning |
| 25 | BO | BOE | Переход в bus off |
| 26 | WDI | WDIE | Watchdog (RAM) |
| 27 | PEA | PEAE | Protocol error в арбитраже |
| 28 | PED | PEDE | Protocol error в данных |
| 29 | ARA | ARAE | Доступ к зарезервированному адресу RAM |
| 31:30 | `res` | `res` | Зарезервированы |

## GFC (0x080) — глобальный фильтр (в задаче RXGFC)

| Биты | Имя | Что делает |
|---|---|---|
| 0 | RRFE | 1 = отбрасывать remote-кадры extended (не класть в FIFO) |
| 1 | RRFS | 1 = отбрасывать remote-кадры standard |
| 3:2 | ANFE | Куда класть non-matching extended: `0=в RX FIFO 0, 1=в RX FIFO 1, 2=отбрасывать, 3=res` |
| 5:4 | ANFS | То же для standard non-matching: `0=FIFO0, 1=FIFO1, 2=отброс, 3=res` |
| 31:6 | `res` | Зарезервированы |

## SIDFC (0x084) / XIDFC (0x088) — адреса фильтров в Message RAM

| Регистр | Биты | Имя | Что делает |
|---|---|---|---|
| SIDFC | 15:2 | FLSSA | Start address стандартных фильтров в Message RAM (в 32-битных словах от начала RAM) |
| SIDFC | 23:16 | LSS | Число стандартных фильтров (`0–128`) |
| SIDFC | 31:24, 1:0 | `res` | Зарезервированы |
| XIDFC | 15:2 | FLESA | Start address extended-фильтров (в словах) |
| XIDFC | 22:16 | LSE | Число extended-фильтров (`0–64`) |
| XIDFC | 31:23, 1:0 | `res` | Зарезервированы |

Сами элементы фильтров лежат в RAM по этим адресам (см. раздел Message RAM).

## XIDAM (0x090) / HPMS (0x094) / NDAT1/2 (0x098/0x09C)

| Регистр | Биты | Имя | Что делает |
|---|---|---|---|
| XIDAM | 28:0 | EIDM | Маска extended-ID для фильтров (какие биты сравнивать) |
| XIDAM | 31:29 | `res` | Зарезервированы |
| HPMS | 5:0 | BIDX | Индекс буфера high-priority сообщения |
| HPMS | 7:6 | MSI | Откуда: `0=не валидно, 1=FIFO0, 2=FIFO1` (`3=res`) |
| HPMS | 14:8 | FIDX | Индекс фильтра, который сработал |
| HPMS | 15 | FLST | 1 = потеряно (HP-сообщение затиралось) |
| HPMS | 31:16 | `res` | Зарезервированы |
| NDAT1 | 31:0 | ND0–ND31 | Флаги «новые данные» для dedicated RX-буферов 0–31 (сброс записью 1 в соотв. бит после чтения буфера) |
| NDAT2 | 31:0 | ND32–ND63 | То же для буферов 32–63 |

## RXF0C/RXF0S/RXF0A (0x0A0–0x0A8), RXBC (0x0AC), RXF1* (0x0B0–0x0B8), RXESC (0x0BC)

RXF0C / RXF1C (конфиг FIFO):

| Биты | Имя | Что делает |
|---|---|---|
| 1:0 | `res` | Зарезервированы |
| 15:2 | F0SA / F1SA | Start address FIFO в Message RAM (в словах) |
| 22:16 | F0S / F1S | Размер FIFO (число элементов `0–64`) |
| 30:24 | F0WM / F1WM | Watermark (порог для `RFxW`) |
| 31 | F0OM / F1OM | Режим: `0=blocking (при полн — терять новые), 1=overwrite (затирать старые)` |

RXF0S / RXF1S (статус, чтение):

| Биты | Имя | Что делает |
|---|---|---|
| 6:0 | F0FL / F1FL | Fill level (сколько элементов сейчас) |
| 7 | `res` | Зарезервирован |
| 13:8 | F0GI / F1GI | Get index (индекс следующего на чтение) |
| 15:14 | `res` | Зарезервированы |
| 21:16 | F0PI / F1PI | Put index (куда положено последнее) |
| 23:22 | `res` | Зарезервированы |
| 24 | F0F / F1F | 1 = FIFO полон |
| 25 | RF0L / RF1L | 1 = потеряно сообщение (переполнение) |
| 31:26 | `res` | Зарезервированы |

RXF0A / RXF1A: `F0AI/F1AI[5:0]` — acknowledge index (запиши `F0GI` после чтения элемента, чтобы освободить место), `31:6 res`.

RXBC: `RBSA[15:2]` — start address dedicated RX-буферов (в словах), `31:16, 1:0 res`.

RXESC (размеры элементов):

| Биты | Имя | Что делает |
|---|---|---|
| 2:0 | F0DS | Размер элемента FIFO0: `0=8, 1=12, 2=16, 3=20, 4=24, 5=32, 6=48, 7=64 байта данных` |
| 6:4 | F1DS | То же для FIFO1 |
| 10:8 | RBDS | То же для dedicated буферов |
| 31:11 | `res` | Зарезервированы |

## TXBC (0x0C0) / TXFQS (0x0C4) / TXESC (0x0C8)

TXBC:

| Биты | Имя | Что делает |
|---|---|---|
| 1:0 | `res` | Зарезервированы |
| 15:2 | TBSA | Start address TX-буферов в RAM (в словах) |
| 21:16 | NDTB | Число dedicated TX-буферов (`0–32`) |
| 23:22 | `res` | Зарезервированы |
| 29:24 | TFQS | Размер TX FIFO/queue (`0–32`) |
| 30 | TFQM | 0 = TX FIFO (порядок отправки = порядок записи), 1 = TX queue (приоритет по ID) |
| 31 | `res` | Зарезервирован |

TXFQS (статус):

| Биты | Имя | Что делает |
|---|---|---|
| 5:0 | TFFL | Сколько свободно в TX FIFO (`0–32`) |
| 7:6 | `res` | Зарезервированы |
| 12:8 | TFGI | Get index TX FIFO |
| 15:13 | `res` | Зарезервированы |
| 20:16 | TFQPI | Put index TX FIFO |
| 21 | TFQF | 1 = TX FIFO полон |
| 31:22 | `res` | Зарезервированы |

TXESC: `TBDS[2:0]` — размер TX-элемента (`0=8 … 7=64`, как RXESC), `31:3 res`.

## TXBRP/TXBAR/TXBCR/TXBTO/TXBCF/TXBTIE/TXBCIE (0x0CC–0x0E4)

Все — битовые маски буферов 31:0 (бит N = буфер N). TXBRP только чтение, остальные R/W:

| Регистр | Имя поля | Что делает |
|---|---|---|
| TXBRP | TRP | 1 = для буфера запрошена передача (pending, чтение) |
| TXBAR | AR | Запись 1 = запросить передачу буфера N (add request). Чтение = какие запрошены |
| TXBCR | CR | Запись 1 = отменить запрос буфера N (cancel). Не отменяет уже идущий кадр |
| TXBTO | TO | 1 = передача буфера N состоялась (transmission occurred, сброс записью 1/чтением + TXBAR) |
| TXBCF | CF | 1 = отмена буфера N выполнена (cancel finished) |
| TXBTIE | TIE | 1 = прерывание `IR.TC` для буфера N |
| TXBCIE | CFIE | 1 = прерывание `IR.TCF` для буфера N |

## TXEFC (0x0F0) / TXEFS (0x0F4) / TXEFA (0x0F8) — TX Event FIFO

TXEFC:

| Биты | Имя | Что делает |
|---|---|---|
| 1:0 | `res` | Зарезервированы |
| 15:2 | EFSA | Start address Event FIFO в RAM (в словах) |
| 21:16 | EFS | Размер Event FIFO (`0–32`) |
| 29:24 | EFWM | Watermark |
| 31:30 | `res` | Зарезервированы |

TXEFS:

| Биты | Имя | Что делает |
|---|---|---|
| 5:0 | EFFL | Fill level (сколько записей) |
| 7:6 | `res` | Зарезервированы |
| 12:8 | EFGI | Get index |
| 15:13 | `res` | Зарезервированы |
| 20:16 | EFPI | Put index |
| 23:21 | `res` | Зарезервированы |
| 24 | EFF | 1 = полон |
| 25 | TEFL | 1 = потеряно |
| 31:26 | `res` | Зарезервированы |

TXEFA: `EFAI[4:0]` — acknowledge (запиши `EFGI` после чтения), `31:5 res`.

## Message RAM — раскладка (база 0x4000AC00, общая на FDCAN1/2)

Все `*SA` — в 32-битных словах от начала RAM. Порядок секций — любой, но без пересечений; размеры зависят от `RXESC/TXESC` (элемент = заголовок 2 слова + данные). Типовая карта:

| Секция | Задаётся | Что лежит |
|---|---|---|
| STD-фильтры | SIDFC.FLSSA, SIDFC.LSS | По 1 слову на фильтр: `SFID1/SFID2/SFEC/SFT` |
| XID-фильтры | XIDFC.FLESA, XIDFC.LSE | По 2 слова на фильтр: `EFID1/EFID2/EFEC/EFT` (+ маска `XIDAM`) |
| RX FIFO 0 | RXF0C.F0SA, RXF0C.F0S, RXESC.F0DS | Элементы приёма FIFO0 |
| RX FIFO 1 | RXF1C.F1SA, RXF1C.F1S, RXESC.F1DS | Элементы приёма FIFO1 |
| RX dedicated | RXBC.RBSA, RXESC.RBDS | До 64 буферов (флаги — NDAT1/2) |
| TX Event FIFO | TXEFC.EFSA, TXEFC.EFS | Записи о переданных (ID + timestamp) |
| TX буферы | TXBC.TBSA, TXBC.NDTB/TFQS, TXESC.TBDS | Dedicated (0..NDTB-1) + FIFO/queue (NDTB..NDTB+TFQS-1) |

Формат RX-элемента в RAM (M_CAN): слово 0 `ID[28:0]+RTR/XTD/ESI`, слово 1 `RXTS[15:0]+DLC[3:0]+BRS/FDF`, далее данные `0–64` байта. TX-элемент аналогично + `ESI/BRS/FDF/EFC/MM[7:0]`. Точные поля слов — по RM Bosch M_CAN (в `stm32h723xx.h` их нет — это RAM, а не регистры).

Пример раскладки (слова, `W = addr/4` от 0x4000AC00): `STD[0..LSS-1] @ FLSSA`, `XID[0..2*LSE-1] @ FLESA`, `RXF0 @ F0SA`, `RXF1 @ F1SA`, `RXB @ RBSA`, `TXEF @ EFSA`, `TXB @ TBSA`. Между секциями оставлять зазор по размеру элемента.

## Алгоритм: INIT → битрейт → фильтры → TX через TXBAR → приём из FIFO

1. Вход в конфигурацию: `CCCR.INIT=1`, ждать пока железо подтвердит (бит держится), затем `CCCR.CCE=1`. Все настройки ниже — только при `INIT+CCE=1`.
2. Номинал (арбитраж): посчитать `NBTP.NBRP/NTSEG1/NTSEG2/NSJW` по формуле выше (напр. 500 кбит/с при `f_can=80 МГц`: `NBRP=9` (`/10`), `NTSEG1=13`, `NTSEG2=2`, `NSJW=1` → `80M/(10*(1+14+3))=444к` — подобрать точнее под кварц; семпл ≈ 83%).
3. Данные FD (если `FDOE/BRSE=1`): `DBTP.DBRP/DTSEG1/DTSEG2/DSJW`, `DBTP.TDC=1`, `TDCR.TDCO/TDCF` (напр. 2 Мбит/с: `DBRP=3` (`/4`), `DTSEG1=7`, `DTSEG2=2`, `DSJW=1` → `80M/(4*(1+8+3))≈1.66M` — подобрать).
4. Режим: `CCCR.FDOE/BRSE` (FD), `CCCR.NISO` (0=ISO), `CCCR.DAR/TXP` по желанию, `CCCR.TEST/LBCK` для самотеста без трансивера.
5. Message RAM: заполнить фильтры в RAM по `FLSSA/FLESA`, настроить `SIDFC/XIDFC`, `GFC` (куда non-matching), `RXF0C/RXF1C/RXBC`, `RXESC/TXESC`, `TXBC` (TBSA/NDTB/TFQS/TFQM), `TXEFC` если нужны подтверждения.
6. Прерывания: `IE` (напр. `RF0NE+TC`), `ILS` (какая линия), `ILE.EINT0/EINT1=1`.
7. Выход: `CCCR.CCE=0`, затем `CCCR.INIT=0` — узел идёт в шину (синхронизация до 11 рецессивных бит).
8. TX: собрать элемент в RAM по `TBSA+N*размер`, затем `TXBAR = (1<<N)`. Ждать `TXBTO&(1<<N)` или `IR.TC`. Подтверждение — читать TX Event FIFO (`TXEFS.EFFL>0` → читать по `EFSA+EFGI` → `TXEFA=EFGI`).
9. RX: ждать `IR.RF0N` (или `RXF0S.F0FL>0`) → читать элемент по `F0SA+F0GI*размер` → `RXF0A = F0GI` (освободить). Ошибки `RF0L` — счётчик потерь.
10. Bus-off: при `PSR.BO=1` — возврат через `CCCR.INIT=1/0` (после 128×11 рецессивных по стандарту) + реинит при необходимости.

Псевдокод (CMSIS):
```c
FDCAN1->CCCR |= (1u << 0);                    // INIT
while (!(FDCAN1->CCCR & 0x1u));
FDCAN1->CCCR |= (1u << 1);                    // CCE
FDCAN1->NBTP = (1u << 25)|(13u << 8)|(2u << 0)|(9u << 16); // NSJW/NTSEG1/NTSEG2/NBRP
FDCAN1->DBTP = (1u << 0)|(2u << 4)|(7u << 8)|(3u << 16)|(1u << 23); // DSJW/DTSEG2/DTSEG1/DBRP/TDC
FDCAN1->TDCR = (8u << 8)|(5u << 0);            // TDCO/TDCF
FDCAN1->CCCR |= (1u << 8)|(1u << 9);           // FDOE+BRSE
// ... RAM: фильтры, RXF0C, TXBC, RXESC/TXESC, GFC ...
FDCAN1->IE = (1u << 0)|(1u << 9);              // RF0NE+TCE
FDCAN1->ILE = (1u << 0);                       // EINT0
FDCAN1->CCCR &= ~(1u << 1);                   // CCE=0
FDCAN1->CCCR &= ~(1u << 0);                   // INIT=0, в шину
// TX буфер 0:
FDCAN1->TXBAR = (1u << 0);
while (!(FDCAN1->TXBTO & (1u << 0)));
```
