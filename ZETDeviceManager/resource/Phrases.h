//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <Dialog_ZET\Translate.h>
#include <singleton.hpp>

//----------------------------------------------------------------------------------------------------------------------
struct PhrasesImpl
{
	CString sPreamplifier = TranslateString(L"ПУ");
#define g_sPreamplifier Phrases::getInstance().sPreamplifier // ПУ 
	 CString sNo = TranslateString(L"Нет");
#define g_sNo Phrases::getInstance().sNo // Нет 
	 CString sYes = TranslateString(L"Да");
#define g_sYes Phrases::getInstance().sYes // Да 


	 //Настройки разное
	 CString sNumber = TranslateString(L"№");
#define g_sNumber Phrases::getInstance().sNumber // №
	 CString sTotalVolume = TranslateString(L"Общий объем");
#define g_sTotalVolume Phrases::getInstance().sTotalVolume // Общий объем
	 CString sInformationRegistrar = TranslateString(L"Информация о регистраторе");
#define g_sInformationRegistrar Phrases::getInstance().sInformationRegistrar// Информация о регистраторе
	 CString sSetUpYourDevice = TranslateString(L"Настроить устройство");
#define g_sSetUpYourDevice Phrases::getInstance().sSetUpYourDevice// Настроить устройство
	 CString sPersonalComputer = TranslateString(L"ПК");
#define g_sPersonalComputer Phrases::getInstance().sPersonalComputer// ПК
	 CString sRefreshInformationAboutDisk = TranslateString(L"Обновите информацию о диске!");
#define g_sRefreshInformationAboutDisk Phrases::getInstance().sRefreshInformationAboutDisk// Обновите информацию о диске!
	 CString sSelectingFolderFilesSignal = TranslateString(L"Выбор папки с файлами сигналов");
#define g_sSelectingFolderFilesSignal Phrases::getInstance().sSelectingFolderFilesSignal// Выбор папки с файлами сигналов
	 CString sSelectingFolderSilentConverter = TranslateString(L"Выбор папки для конвертации");
#define g_sSelectingFolderSilentConverter Phrases::getInstance().sSelectingFolderSilentConverter// Выбор папки для конвертации
	 CString sHowToUse = TranslateString(L"Как использовать");
#define g_sHowToUse Phrases::getInstance().sHowToUse// Как использовать
	 CString sRegistrationSamplingFrequency = TranslateString(L"Регистрация на ZET048 при частоте дискретизации 2500 Гц не поддерживается. Необходимо изменить частоту дискретизации АЦП.");
#define g_sRegistrationSamplingFrequency Phrases::getInstance().sRegistrationSamplingFrequency// Регистрация на ZET048 при частоте дискретизации 2500 Гц не поддерживается. Необходимо изменить частоту дискретизации АЦП.
	 CString sProperties = TranslateString(L"Свойства");
#define g_sProperties Phrases::getInstance().sProperties// Свойства
	 CString sPropertiesExport = TranslateString(L"Экспорт настроек") + L" ...";
#define g_sPropertiesExport Phrases::getInstance().sPropertiesExport// Экспорт настроек...
	 CString sPropertiesImport = TranslateString(L"Импорт настроек") + L" ...";
#define g_sPropertiesImport Phrases::getInstance().sPropertiesImport// Импорт настроек...
	 CString sPropertiesDefault = TranslateString(L"Настройки по умолчанию");
#define g_sPropertiesDefault Phrases::getInstance().sPropertiesDefault// Настройки по умолчанию
	 CString sServiceMode = TranslateString(L"Сервисный режим");
#define g_sServiceMode Phrases::getInstance().sServiceMode// Сервисный режим
	 CString sZETDeviceManager = TranslateString(L"Диспетчер устройств ZET");
#define g_sZETDeviceManager Phrases::getInstance().sZETDeviceManager// Диспетчер устройств ZET
	 CString sICP = TranslateString(L"ICP");
#define g_sICP Phrases::getInstance().sICP // ICP
	 CString sSynchronization = TranslateString(L"Синхронизация");
#define g_sSynchronization Phrases::getInstance().sSynchronization // Синхронизация
	 CString sVoltage = TranslateString(L"Напряжение");
#define g_sVoltage Phrases::getInstance().sVoltage // Напряжение
	 CString sInformationNotAvailable = TranslateString(L"Информация недоступна");
#define g_sInformationNotAvailable Phrases::getInstance().sInformationNotAvailable // Информация недоступна
	 CString sIncorrectData = TranslateString(L"Некорректные данные");
#define g_sIncorrectData Phrases::getInstance().sIncorrectData // Некорректные данные
	 CString sOptions = TranslateString(L"Параметры");
#define g_sOptions Phrases::getInstance().sOptions // Параметры
	 CString sUnnamed = TranslateString(L"Безымянный");
#define g_sUnnamed Phrases::getInstance().sUnnamed // Безымянный
	 CString sDeviceType = TranslateString(L"Тип устройства");
#define g_sDeviceType Phrases::getInstance().sDeviceType// Тип устройства
	 CString sVersion = TranslateString(L"Версия");
#define g_sVersion Phrases::getInstance().sVersion// Версия
	 CString sStop = TranslateString(L"Стоп");
#define g_sStop Phrases::getInstance().sStop // Стоп
	 CString sActuator = TranslateString(L"Эл. контроль");
#define g_sActuator Phrases::getInstance().sActuator // Эл. контроль
	 CString sErrorDownload = TranslateString(L"Не удалось скачать файл");
#define g_sErrorDownload Phrases::getInstance().sErrorDownload // Не удалось скачать файл
	 CString sOpenFile = TranslateString(L"Открыть файл для просмотра свойств устройств");
#define g_sOpenFile Phrases::getInstance().sOpenFile // Открыть файл для просмотра свойств устройств	
	 CString sErrorReadConfiguration = TranslateString(L"Ошибка чтения конфигурации датчика. Необходимо выполнить очистку конфигурации 7xxx (кликнуть на иконку в верхнем левом углу панели ZETLab и нажать \"Очистить конфигурацию ZET7xxx\")");
#define g_sErrorReadConfiguration Phrases::getInstance().sErrorReadConfiguration // Ошибка чтения конфигурации датчика
	 CString sGroupProperties = TranslateString(L"Групповая настройка каналов");
#define g_sGroupProperties Phrases::getInstance().sGroupProperties // Групповая настройка каналов
	 CString s_selftest = TranslateString(L"Самоконтроль");
#define g_s_selftest Phrases::getInstance().s_selftest // Самоконтроль
	 CString s_mseed_metadata = TranslateString(L"Данные mseed");
#define g_s_mseed_metadata Phrases::getInstance().s_mseed_metadata // Данные mseed
	 
	 //Настройки измерительного канала
	 CString sMeasureName = TranslateString(L"Измерительный канал");
#define g_sMeasureName Phrases::getInstance().sMeasureName// Измерительный канал	 
	 CString sName = TranslateString(L"Название");
#define g_sName Phrases::getInstance().sName// Название
	 CString sNameLocation = TranslateString(L"LOC (2 симв.)");
#define g_sNameLocation Phrases::getInstance().sNameLocation// LOC (2 симв.)
	 CString sNameChannel = TranslateString(L"CHA (3 симв.)");
#define g_sNameChannel Phrases::getInstance().sNameChannel// CHA (3 симв.)
	 CString sComment = TranslateString(L"Комментарий");
#define g_sComment Phrases::getInstance().sComment// Комментарий
	 CString sMeasureUnit = TranslateString(L"Единица измерения");
#define g_sMeasureUnit Phrases::getInstance().sMeasureUnit// Единица измерения
	 CString sConstantIncrease = TranslateString(L"КУ внешнего усилителя");
#define g_sConstantIncrease Phrases::getInstance().sConstantIncrease// КУ внешнего усилителя
	 CString sConstantIncrease2 = TranslateString(L"Коэффициент усиления");
#define g_sConstantIncrease2 Phrases::getInstance().sConstantIncrease2// Коэффициент усиления
	 CString sConstantIncrease3 = TranslateString(L"Коэффициент усиления внешнего усилителя");
#define g_sConstantIncrease3 Phrases::getInstance().sConstantIncrease3// Коэффициент усиления внешнего усилителя
	 CString sAmplifier = TranslateString(L"КУ");
#define g_sAmplifier Phrases::getInstance().sAmplifier// КУ
	 CString sDifferential = TranslateString(L"Дифференциальный");
#define g_sDifferential Phrases::getInstance().sDifferential// Дифференциальный
	 CString sReferenceValueLevel = TranslateString(L"Опорное значение");
#define g_sReferenceValueLevel Phrases::getInstance().sReferenceValueLevel// Опорное значение
	 CString sIntegratedLevelSignal = TranslateString(L"Интегральный уровень сигнала");
#define g_sIntegratedLevelSignal Phrases::getInstance().sIntegratedLevelSignal// Интегральный уровень сигнала
	 CString sRange = TranslateString(L"Диапазон");
#define g_sRange Phrases::getInstance().sRange// Диапазон
	 CString sTo = TranslateString(L"до");
#define g_sTo Phrases::getInstance().sTo// до
	 CString sChannelSensitivity = TranslateString(L"Чувствительность");
#define g_sChannelSensitivity Phrases::getInstance().sChannelSensitivity// Чувствительность
	 CString sResistor = TranslateString(L"Сопротивление тензорезистора");
#define g_sResistor Phrases::getInstance().sResistor// Сопротивление тензорезистора
	 CString sTensoCoeff = TranslateString(L"Коэффициент тензочувствительности");
#define g_sTensoCoeff Phrases::getInstance().sTensoCoeff// Коэффициент тензочувствительности
	 CString sTensoUnit = TranslateString(L"Ед. изм. деформации/напряжения");
#define g_sTensoUnit Phrases::getInstance().sTensoUnit// Ед. изм. деформации/напряжения
	 CString sYoungMod = TranslateString(L"Модуль Юнга");
#define g_sYoungMod Phrases::getInstance().sYoungMod// Модуль Юнга
	 CString sSensorRange = TranslateString(L"Предел измерений");
#define g_sSensorRange Phrases::getInstance().sSensorRange// Предел измерений
	 CString sSensorOutput = TranslateString(L"Номинальный сигнал, мВ/В");
#define g_sSensorOutput Phrases::getInstance().sSensorOutput// Номинальный сигнал, мВ/В
	 CString sTensoResistorMode = TranslateString(L"Режим измерения");
#define g_sTensoResistorMode Phrases::getInstance().sTensoResistorMode// Режим измерения
	 CString sPoissonRatio = TranslateString(L"Коэффициент Пуассона");
#define g_sPoissonRatio Phrases::getInstance().sPoissonRatio// Коэффициент Пуассона
	 CString sElasticModulus = TranslateString(L"Модуль упругости, МПа");
#define g_sElasticModulus Phrases::getInstance().sElasticModulus// Модуль упругости, МПа
	 CString sOuterDiameter = TranslateString(L"Диаметр вала, м");
#define g_sOuterDiameter Phrases::getInstance().sOuterDiameter// Диаметр вала, м
	 CString sInnerDiameter = TranslateString(L"Внутренний диаметр вала, м");
#define g_sInnerDiameter Phrases::getInstance().sInnerDiameter// Внутренний диаметр вала, м
	 	 	 
	 CString sChannelSensitivitySensor = TranslateString(L"Чувствительность датчика");
#define g_sChannelSensitivitySensor Phrases::getInstance().sChannelSensitivitySensor// Чувствительность датчика

	 //Настройки заряда
	 CString sBatteryСharge = TranslateString(L"Заряд батареи");
#define g_sBatteryСharge Phrases::getInstance().sBatteryСharge // Заряд батареи
	 CString sCapacity = TranslateString(L"Заряд");
#define g_sCapacity Phrases::getInstance().sCapacity // Заряд
	 CString sBatteryChargeLevel = TranslateString(L"Уровень заряда батареи");
#define g_sBatteryChargeLevel Phrases::getInstance().sBatteryChargeLevel // Уровень заряда батареи
	 CString sBatteryStatus = TranslateString(L"Состояние аккумуляторной батареи");
#define g_sBatteryStatus Phrases::getInstance().sBatteryStatus // Состояние аккумуляторной батареи
	 CString sDeviceUptime = TranslateString(L"Время работы устройства");
#define g_sDeviceUptime Phrases::getInstance().sDeviceUptime // Время работы устройства
	 CString sDriverUptime = TranslateString(L"Время работы драйвера");
#define g_sDriverUptime Phrases::getInstance().sDriverUptime // Время работы драйвера

	 //Настройка цифрового порта
	 CString sDigitalPort = TranslateString(L"Цифровой порт");
#define g_sDigitalPort Phrases::getInstance().sDigitalPort// Цифровой порт
	 CString sLevelIndicationLine = TranslateString(L"Индикация уровня на линии");
#define g_sLevelIndicationLine Phrases::getInstance().sLevelIndicationLine // Индикация уровня на линии
	 CString sControlStatusOutput = TranslateString(L"Управление состоянием на выходе");
#define g_sControlStatusOutput Phrases::getInstance().sControlStatusOutput // Управление состоянием на выходе
	 CString sExitEnableDisable = TranslateString(L"Разрешение/запрет выхода");
#define g_sExitEnableDisable Phrases::getInstance().sExitEnableDisable // Разрешение/запрет выхода


	 // Настройка Ethernet
	 CString sMainGateway = TranslateString(L"Основной шлюз");
#define g_sMainGateway Phrases::getInstance().sMainGateway // Основной шлюз
	 CString sPort = TranslateString(L"Порт");
#define g_sPort Phrases::getInstance().sPort // Порт
	 CString sMACAddress = TranslateString(L"MAC-адрес");
#define g_sMACAddress Phrases::getInstance().sMACAddress // MAC-адрес
	 CString sTimeout = TranslateString(L"Таймаут");
#define g_sTimeout Phrases::getInstance().sTimeout // Таймаут
	 CString sTransmissionMode = TranslateString(L"Режим передачи");
#define g_sTransmissionMode Phrases::getInstance().sTransmissionMode // Режим передачи
	 CString sDuplex = TranslateString(L"Дуплекс");
#define g_sDuplex Phrases::getInstance().sDuplex // Дуплекс
	 CString sHalfDuplex = TranslateString(L"Полудуплекс");
#define g_sHalfDuplex Phrases::getInstance().sHalfDuplex // Полудуплекс
	 CString sBitRate = TranslateString(L"Скорость передачи");
#define g_sBitRate Phrases::getInstance().sBitRate // Скорость передачи
	 CString s_dynamic_ip = TranslateString(L"DHCP");
#define g_s_dynamic_ip Phrases::getInstance().s_dynamic_ip // DHCP
	 CString s_ftp = TranslateString(L"FTP");
#define g_s_ftp Phrases::getInstance().s_ftp // FTP

	 //Настройки IP-адресов
	 CString sIPaddressСomputer = TranslateString(L"IP-адреса компьютера");
#define g_sIPaddressСomputer Phrases::getInstance().sIPaddressСomputer // IP-адреса компьютера 
	 CString sIPAddress = TranslateString(L"IP-адрес");
#define g_sIPAddress Phrases::getInstance().sIPAddress // IP-адрес 
	 CString sAddress = TranslateString(L"адрес");
#define g_sAddress Phrases::getInstance().sAddress // адрес
	 CString sIPConflict = TranslateString(L"Конфликт IP");
#define g_sIPConflict Phrases::getInstance().sIPConflict // Конфликт IP"
	 
	 //Электрический контроль
	 CString sActuatorState = TranslateString(L"Включение электрического контроля по датчикам");
#define g_sActuatorState Phrases::getInstance().sActuatorState // Включение электрического контроля по датчикам

	 //Список параметров
	 CString sListParameters = TranslateString(L"Список параметров");
#define g_sListParameters Phrases::getInstance().sListParameters // Список параметров
	 CString sUseType = TranslateString(L"Задействовать тип");
#define g_sUseType Phrases::getInstance().sUseType // Задействовать тип
	 CString sTypeActivate = TranslateString(L"Активировать тип");
#define g_sTypeActivate Phrases::getInstance().sTypeActivate // Активировать тип
	 CString sVibrometer = TranslateString(L"Виброметр");
#define g_sVibrometer Phrases::getInstance().sVibrometer // Виброметр
	 CString sRegistrator = TranslateString(L"Регистратор");
#define g_sRegistrator Phrases::getInstance().sRegistrator// Регистратор
	 CString g_sNumberSensor = TranslateString(L"Датчик");
#define g_sNumberSensor Phrases::getInstance().g_sNumberSensor // Датчик
	 CString sRecordingErrorOccurred = TranslateString(L"Произошла ошибка записи");
#define g_sRecordingErrorOccurred Phrases::getInstance().sRecordingErrorOccurred // Произошла ошибка записи 
	 CString sSRSPDSensitivity = TranslateString(L"Чувств.");
#define g_sSRSPDSensitivity Phrases::getInstance().sSRSPDSensitivity // Чувств.
	 CString sSRSPDUnit = TranslateString(L"Ед. изм.");
#define g_sSRSPDUnity Phrases::getInstance().sSRSPDUnit // Ед. изм.
	 CString sSRSPDAmpCoef = TranslateString(L"К. усил.");
#define g_sSRSPDAmpCoef Phrases::getInstance().sSRSPDAmpCoef // К. усил.
	 CString sSRSPDPreamplifier = TranslateString(L"Предвар. усилитель");
#define g_sSRSDPreamplifier Phrases::getInstance().sSRSPDPreamplifier // Предвар. усилитель
	 CString sSRSPDFreqResp = TranslateString(L"АЧХ");
#define g_sSRSPDFreqResp Phrases::getInstance().sSRSPDFreqResp // АЧХ
	 CString sSRSPDAudioNoiseMeter = TranslateString(L"Шумомер");
#define g_sSRSPDAudioNoiseMeter Phrases::getInstance().sSRSPDAudioNoiseMeter // Шумомер

	 //Настройки маски
	 CString sSubnetmask = TranslateString(L"маска подсети");
#define g_sSubnetmask Phrases::getInstance().sSubnetmask // маска подсети 
	 CString sSubnetMaskBig = TranslateString(L"Маска подсети");
#define g_sSubnetMaskBig Phrases::getInstance().sSubnetMaskBig // Маска подсети 

	 //Настройки подключения
	 CString sStatus = TranslateString(L"Состояние");
#define g_sStatus Phrases::getInstance().sStatus // Состояние
	 CString sDisable = TranslateString(L"Отключить");
#define g_sDisable Phrases::getInstance().sDisable// Отключить
	 CString sInvert = TranslateString(L"Инвертировать");
#define g_sInvert Phrases::getInstance().sInvert// Инвертировать
	 CString sEnable = TranslateString(L"Задействовать");
#define g_sEnable Phrases::getInstance().sEnable// Задействовать
	 CString g_sAccept = TranslateString(L"Прием данных");
#define g_sAccept Phrases::getInstance().g_sAccept// Прием данных
	 CString sNoNetworkConnections = TranslateString(L"Сетевые подключения отсутствуют");
#define g_sNoNetworkConnections Phrases::getInstance().sNoNetworkConnections // Сетевые подключения отсутствуют 
	 CString sOn = TranslateString(L"Включен");
#define g_sOn Phrases::getInstance().sOn // Включен 
	 CString sOff = TranslateString(L"Отключен");
#define g_sOff Phrases::getInstance().sOff // Отключен 
	 CString sActivated = TranslateString(L"Включено");
#define g_sActivated Phrases::getInstance().sActivated // Включено 
	 CString sDisabled = TranslateString(L"Отключено");
#define g_sDisabled Phrases::getInstance().sDisabled // Отключено

	 CString sControl = TranslateString(L"Управление");
#define g_sControl Phrases::getInstance().sControl // Управление 
	 CString sConnected = TranslateString(L"Подключено");
#define g_sConnected Phrases::getInstance().sConnected // Подключено
	 CString sConnecting = TranslateString(L"Подключается");
#define g_sConnecting Phrases::getInstance().sConnecting // Подключается
	 CString sBusy = TranslateString(L"Подключено к");
#define g_sBusy Phrases::getInstance().sBusy // Подключено к
	 CString sBusyStatus = TranslateString(L"Занято");
#define g_sBusyStatus Phrases::getInstance().sBusyStatus // Занято
	 CString sBusyAnotherPC = TranslateString(L"Подключено к другому ПК");
#define g_BusyAnotherPC Phrases::getInstance().sBusyAnotherPC // Подключено к другому ПК
	 CString sAccepted = TranslateString(L"Прием данных");
#define g_sAccepted Phrases::getInstance().sAccepted // Прием данных
	 CString sAvailable = TranslateString(L"Свободно");
#define g_sAvailable Phrases::getInstance().sAvailable // Свободно
	 CString sAdd = TranslateString(L"Добавить");
#define g_sAdd Phrases::getInstance().sAdd // Добавить
	 CString sDelete = TranslateString(L"Удалить");
#define g_sDelete Phrases::getInstance().sDelete // Удалить
	 CString sApply = TranslateString(L"Применить");
#define g_sApply Phrases::getInstance().sApply // Применить
	 CString sCancel = TranslateString(L"Отменить");
#define g_sCancel Phrases::getInstance().sCancel // Отменить
	 CString sFormat = TranslateString(L"Форматировать");
#define g_sFormat Phrases::getInstance().sFormat // Форматировать
	 CString sDownload = TranslateString(L"Скачать");
#define g_sDownload Phrases::getInstance().sDownload // Скачать
	 CString sSave = TranslateString(L"Сохранить");
#define g_sSave Phrases::getInstance().sSave // Сохранить	
	 CString sOption = TranslateString(L"Выбор");
#define g_sOption Phrases::getInstance().sOption // Выбор
	 CString sOpen = TranslateString(L"Открыть");
#define g_sOpen Phrases::getInstance().sOpen // Открыть
	 CString sConvert = TranslateString(L"Конвертировать");
#define g_sConvert Phrases::getInstance().sConvert // Конвертировать
	 CString sCopy = TranslateString(L"Копировать");
#define g_sCopy Phrases::getInstance().sCopy // Копировать
	 CString sPaste = TranslateString(L"Вставить");
#define g_sPaste Phrases::getInstance().sPaste // Вставить
	 CString sBlink = TranslateString(L"Выделить");
#define g_sBlink Phrases::getInstance().sBlink // Выделить
	 CString sEquipmentSetupWizard = TranslateString(L"Мастер настройки оборудования");
#define g_sEquipmentSetupWizard Phrases::getInstance().sEquipmentSetupWizard // Мастер настройки оборудования
	 CString sWriteConfiguration = TranslateString(L"Записать конфигурацию");
#define g_sWriteConfiguration Phrases::getInstance().sWriteConfiguration//Записать конфигурацию
	 CString sAddConfiguration = TranslateString(L"Добавить конфигурацию");
#define g_sAddConfiguration Phrases::getInstance().sAddConfiguration//Добавить конфигурацию
	 CString sWriteAllConfiguration = TranslateString(L"Записать конфигурации линии");
#define g_sWriteAllConfiguration Phrases::getInstance().sWriteAllConfiguration // Записать конфигурации линии
	 CString sTreeDevices= TranslateString(L"Дерево устройств");
#define g_sTreeDevices Phrases::getInstance().sTreeDevices // Дерево устройств
	 CString sWaitingReceivedData = TranslateString(L"Ожидание получения данных");
#define g_sWaitingReceivedData Phrases::getInstance().sWaitingReceivedData // Ожидание получения данных

	 //Настройки генератора
	 CString sGenerator = TranslateString(L"Генератор");
#define g_sGenerator Phrases::getInstance().sGenerator // Генератор 
	 CString sOffsetGenerator = TranslateString(L"Смещение генератора");
#define g_sOffsetGenerator Phrases::getInstance().sOffsetGenerator // Смещение генератора 

	 CString sOffsetGeneratorOFF = TranslateString(L"Встроенный генератор отключен");
#define g_sOffsetGeneratorOFF Phrases::getInstance().sOffsetGeneratorOFF // Встроенный генератор отключен

	 CString sGeneratorFrequency = TranslateString(L"Частота генератора");
#define g_sGeneratorFrequency Phrases::getInstance().sGeneratorFrequency // Частота генератора 
	 CString sGeneratorLevel = TranslateString(L"Уровень генератора");
#define g_sGeneratorLevel Phrases::getInstance().sGeneratorLevel // Уровень генератора 
	 CString sIncludeGeneratorSine = TranslateString(L"Включать генератор синуса");
#define g_sIncludeGeneratorSine Phrases::getInstance().sIncludeGeneratorSine // Включать генератор синуса 
	 CString sGeneratorUsedAnotherProgram = TranslateString(L"Канал генератора используется другой программой");
#define g_sGeneratorUsedAnotherProgram Phrases::getInstance().sGeneratorUsedAnotherProgram // Канал генератора используется другой программой

	 //Настройки меню 
	 CString sEditSensorDatabase = TranslateString(L"Редактировать базу датчиков");
#define g_sEditSensorDatabase Phrases::getInstance().sEditSensorDatabase // Редактировать базу датчиков
	 CString sEditDatabaseThirdPartyDevices = TranslateString(L"Редактировать базу сторонних устройств");
#define g_sEditDatabaseThirdPartyDevices Phrases::getInstance().sEditDatabaseThirdPartyDevices // Редактировать базу сторонних устройств
	 CString sHelp = TranslateString(L"Справка");
#define g_sHelp Phrases::getInstance().sHelp// &Справка
	 CString sSeeInformation = TranslateString(L"Просмотреть справку");
#define g_sSeeInformation Phrases::getInstance().sSeeInformation// Просмотреть справку
	 CString sMenuFile = TranslateString(L"Файл");
#define g_sMenuFile Phrases::getInstance().sMenuFile// Файл
	 CString sMenuExit = TranslateString(L"Выход");
#define g_sMenuExit Phrases::getInstance().sMenuExit// Выход
	 CString sMenuAcions = TranslateString(L"Действия");
#define g_sMenuAcions Phrases::getInstance().sMenuAcions// Действия
	 CString sMenuView = TranslateString(L"Вид");
#define g_sMenuView Phrases::getInstance().sMenuView// Вид
	 CString sUpdate = TranslateString(L"Обновить");
#define g_sUpdate Phrases::getInstance().sUpdate // Обновить
	 CString sMenuShort = TranslateString(L"Кратко");
#define g_sMenuShort Phrases::getInstance().sMenuShort // Кратко
	 CString sMenuDetails = TranslateString(L"Подробно");
#define g_sMenuDetails Phrases::getInstance().sMenuDetails // Подробно
	 CString sDescription = TranslateString(L"Описание");
#define g_sDescription Phrases::getInstance().sDescription // Описание
	 CString sIdentification = TranslateString(L"Идентификация");
#define g_sIdentification Phrases::getInstance().sIdentification // Идентификация
	 CString sStreamData = TranslateString(L"Потоковые данные");
#define g_sStreamData Phrases::getInstance().sStreamData // Потоковые данные
	 CString sMeasureData = TranslateString(L"Измеренные данные");
#define g_sMeasureData Phrases::getInstance().sMeasureData // Измеренные данные

	 //Настройки датчиков

	 CString sListProfileSensors = TranslateString(L"Список профилей датчиков");
#define g_sListProfileSensors Phrases::getInstance().sListProfileSensors // Список профилей датчиков
	 CString sProfileSelectionSensor = TranslateString(L"Выбор профилей датчиков");
#define g_sProfileSelectionSensor Phrases::getInstance().sProfileSelectionSensor // Выбор профилей датчиков
	 CString sProfileSensor = TranslateString(L"Профили датчиков");
#define g_sProfileSensor Phrases::getInstance().sProfileSensor // Профили датчиков
	 CString sSensorsDatabase = TranslateString(L"База датчиков");
#define g_sSensorsDatabase Phrases::getInstance().sSensorsDatabase // База датчиков
	 CString sErrorReadingSensor = TranslateString(L"Ошибка чтения датчика");
#define g_sErrorReadingSensor Phrases::getInstance().sErrorReadingSensor // Ошибка чтения датчика
	 CString sSensorEmpty = TranslateString(L"Датчик пустой");
#define g_sSensorEmpty Phrases::getInstance().sSensorEmpty // Датчик пустой
	 CString sSensorNotDetected = TranslateString(L"Датчик не обнаружен");
#define g_sSensorNotDetected Phrases::getInstance().sSensorNotDetected // Датчик не обнаружен
	 CString sSensorNotSupported = TranslateString(L"Датчик не поддерживается");
#define g_sSensorNotSupported Phrases::getInstance().sSensorNotSupported // Датчик не поддерживается
	 CString sDataSensorIncorrectlyRecorded = TranslateString(L"Данные в датчике записаны некорректно");
#define g_sDataSensorIncorrectlyRecorded Phrases::getInstance().sDataSensorIncorrectlyRecorded // Данные в датчике записаны некорректно
	 CString sDetailedShort = TranslateString(L"Подробно/кратко");
#define g_sDetailedShort Phrases::getInstance().sDetailedShort // Подробно/кратко
	 CString sSatellites = TranslateString(L"спут");
#define g_sSatellites Phrases::getInstance().sSatellites // спут
	 CString g_sNumberSatelites = TranslateString(L"Количество спутников");
#define g_sNumberSatelites Phrases::getInstance().g_sNumberSatelites // Количество спутников


	 //Настройки АЦП и ЦАП
	 CString sControlInternalDAC = TranslateString(L"Управление внутренним ЦАП");
#define g_sControlInternalDAC Phrases::getInstance().sControlInternalDAC // Управление внутренним ЦАП 
	 CString sParametersInternalDAC = TranslateString(L"Параметры внутреннего ЦАП");
#define g_sParametersInternalDAC Phrases::getInstance().sParametersInternalDAC // Параметры внутреннего ЦАП 
	 CString sSetADCDACSamplingRate = TranslateString(L"Задать частоту дискретизации АЦП и ЦАП, настроить цифровой порт, установить IP - адрес и параметры синхронизации.");
#define g_sSetADCDACSamplingRate Phrases::getInstance().sSetADCDACSamplingRate // Задать частоту дискретизации АЦП и ЦАП, настроить цифровой порт, установить IP - адрес и параметры синхронизации. 
	 CString sADC = TranslateString(L"АЦП");
#define g_sADC  Phrases::getInstance().sADC  // АЦП
	 CString sDAC = TranslateString(L"ЦАП");
#define g_sDAC  Phrases::getInstance().sDAC  // ЦАП

	 //Настройки частоты дискретизции
	 CString sSamplingFrequencyDevice = TranslateString(L"Частота дискретизации устройства");
#define g_sSamplingFrequencyDevice Phrases::getInstance().sSamplingFrequencyDevice // Частота дискретизации устройства
	 CString sSamplingFrequency = TranslateString(L"Частота дискретизации");
#define g_sSamplingFrequency Phrases::getInstance().sSamplingFrequency // Частота дискретизации

	 //Настройка калибровки 
	 CString sCalibration = TranslateString(L"Калибровка");
#define g_sCalibration Phrases::getInstance().sCalibration // Калибровка (Enum)
	 CString sWhat = TranslateString(L"ЧТО?");
#define g_sWhat Phrases::getInstance().sWhat// Что? (Enum)
	 CString sPoint = TranslateString(L"Точка");
#define g_sPoint Phrases::getInstance().sPoint // Точка 
	 CString sMeasuredValue = TranslateString(L"Измеренное значение");
#define g_sMeasuredValue Phrases::getInstance().sMeasuredValue // Измеренное значение
	 CString sReferenceValue = TranslateString(L"Эталонное значение");
#define g_sReferenceValue Phrases::getInstance().sReferenceValue // Эталонное значение
	 CString sAmendmentFile = TranslateString(L"Файл поправки");
#define g_sAmendmentFile Phrases::getInstance().sAmendmentFile // Эталонное значение





	 //Настройки автономного регистратора
	 CString sSettingRecorderParameters = TranslateString(L"Настройка регистратора");
#define g_sSettingRecorderParameters Phrases::getInstance().sSettingRecorderParameters// Настройка регистратора
	 CString sTimeSetting = TranslateString(L"Настройка времени");
#define g_sTimeSetting Phrases::getInstance().sTimeSetting// Настройка времени
	 CString sWantFormatMemoryCard = TranslateString(L"Вы действительно хотите отформатировать карту памяти?");
#define g_sWantFormatMemoryCard Phrases::getInstance().sWantFormatMemoryCard // Вы действительно хотите отформатировать карту памяти? 
	 CString sMemoryCardFormatting = TranslateString(L"Форматирование карты памяти");
#define g_sMemoryCardFormatting Phrases::getInstance().sMemoryCardFormatting // Форматирование карты памяти
	 CString sWantDeleteFiles = TranslateString(L"Вы действительно хотите удалить эти файлы?");
#define g_sWantDeleteFiles Phrases::getInstance().sWantDeleteFiles // Вы действительно хотите удалить эти файлы?
	 CString sConfirmationDeletion = TranslateString(L"Подтверждение удаления");
#define g_sConfirmationDeletions Phrases::getInstance().sConfirmationDeletion // Подтверждение удаления
	 CString sDiscIsEmpty = TranslateString(L"Диск пуст");
#define g_sDiscIsEmpty Phrases::getInstance().sDiscIsEmpty // Диск пуст
	 CString sFormatIsNotSupported = TranslateString(L"Формат не поддерживается");
#define g_sFormatIsNotSupported Phrases::getInstance().sFormatIsNotSupported // Формат не поддерживается
	 CString sSize = TranslateString(L"Разм");
#define g_sSize Phrases::getInstance().sSize // Разм
	 CString sSizeUsed = TranslateString(L"Исп");
#define g_sSizeUsed Phrases::getInstance().sSizeUsed // Исп
	 CString sSizeAvailable = TranslateString(L"Св");
#define g_sSizeAvailable Phrases::getInstance().sSizeAvailable // Св
	 CString sWaitingDataCopying = TranslateString(L"Ожидание копирования данных");
#define g_sWaitingDataCopying Phrases::getInstance().sWaitingDataCopying // Ожидание копирования данных
	 CString sSaveByUSB = TranslateString(L"по USB");
#define g_sSaveByUSB Phrases::getInstance().sSaveByUSB// по USB
	 CString sSaveByEthernet = TranslateString(L"по Ethernet");
#define g_sSaveByEthernet Phrases::getInstance().sSaveByEthernet// по Ethernet
	 CString sCopyProcessFinish = TranslateString(L"Ожидайте окончания процесса копирования");
#define g_sCopyProcessFinish Phrases::getInstance().sCopyProcessFinish// Ожидайте окончания процесса копирования
	 CString sRegistrationParameters = TranslateString(L"Параметры регистрации");
#define g_sRegistrationParameters Phrases::getInstance().sRegistrationParameters// Параметры регистрации
	 CString sAdjustmentExactGeneratorGPS = TranslateString(L"Подстройка точного генератора по GPS");
#define g_sAdjustmentExactGeneratorGPS Phrases::getInstance().sAdjustmentExactGeneratorGPS// Подстройка точного генератора по GPS
	 CString sContinuousRecording = TranslateString(L"Непрерывная запись");
#define g_sContinuousRecording Phrases::getInstance().sContinuousRecording// Непрерывная запись
	 CString sStartTimeDate = TranslateString(L"Дата и время старта");
#define g_sStartTimeDate Phrases::getInstance().sStartTimeDate// Дата и время старта
	 CString sInitialFront = TranslateString(L"Фронт запуска");
#define g_sInitialFront Phrases::getInstance().sInitialFront// Фронт запуска
	 CString sStartEvent = TranslateString(L"Запуск по событию");
#define g_sStartEvent Phrases::getInstance().sStartEvent// Запуск по событию
	 CString sStartTimer = TranslateString(L"Запуск по таймеру");
#define g_sStartTimer Phrases::getInstance().sStartTimer// Запуск по таймеру
	 CString sExternalAnalogSignal = TranslateString(L"Внешний аналоговый сигнал");
#define g_sExternalAnalogSignal Phrases::getInstance().sExternalAnalogSignal// Внешний аналоговый сигнал
	 CString sExternalDigitalSignal = TranslateString(L"Внешний цифровой сигнал");
#define g_sExternalDigitalSignal Phrases::getInstance().sExternalDigitalSignal// Внешний цифровой сигнал
	 CString sLevelActive = TranslateString(L"Уровень активного");
#define g_sLevelActive Phrases::getInstance().sLevelActive// Уровень активного
	 CString sChannel = TranslateString(L"Канал");
#define g_sChannel Phrases::getInstance().sChannel// Канал
	 CString sThresholdLevel = TranslateString(L"Уровень порога");
#define g_sThresholdLevel Phrases::getInstance().sThresholdLevel// Уровень порога
	 CString sUpperLowerLevel = TranslateString(L"Выше/ниже уровня");
#define g_sUpperLowerLevel Phrases::getInstance().sUpperLowerLevel// Выше/ниже уровня
	 CString sLevelExceeding = TranslateString(L"Превышение уровня");
#define g_sLevelExceeding Phrases::getInstance().sLevelExceeding// Превышение уровня
	 CString sTracingLevel = TranslateString(L"Отслеживать уровень");
#define g_sTracingLevel Phrases::getInstance().sTracingLevel// Отслеживать уровень
	 CString sBitNumber = TranslateString(L"Номер бита");
#define g_sBitNumber Phrases::getInstance().sBitNumber// Номер бита
	 CString sUnitMeasure = TranslateString(L"е.и.");
#define g_sUnitMeasure Phrases::getInstance().sUnitMeasure// е.и.
	 CString sPrimary = TranslateString(L"Первичные");
#define g_sPrimary Phrases::getInstance().sPrimary// Первичные
	 CString sSecondary = TranslateString(L"Вторичные");
#define g_sSecondary Phrases::getInstance().sSecondary// Вторичные
	 CString sAdjustingJobData = TranslateString(L"Настройка работы с данными");
#define g_sAdjustingJobData Phrases::getInstance().sAdjustingJobData// Настройка работы с данными
	 CString sSignalGeneration = TranslateString(L"Генерация сигнала");
#define g_sSignalGeneration Phrases::getInstance().sSignalGeneration// Генерация сигнала
	 CString sFrequency = TranslateString(L"Частота");
#define g_sFrequency Phrases::getInstance().sFrequency// Частота
	 CString sOffset = TranslateString(L"Смещение");
#define g_sOffset Phrases::getInstance().sOffset// Смещение
	 CString sOffsetDC = TranslateString(L"Смещение пост. сост.");
#define g_sOffsetDC Phrases::getInstance().sOffsetDC// Смещение пост. сост.

	 CString sOffsetDC2 = TranslateString(L"Смещение постоянной составляющей");
#define g_sOffsetDC2 Phrases::getInstance().sOffsetDC2// Смещение постоянной составляющей

	 CString sOffsetZero = TranslateString(L"Смещение нуля");
#define g_sOffsetZero Phrases::getInstance().sOffsetZero// Смещение нуля
	 CString sLevel = TranslateString(L"Уровень");
#define g_sLevel Phrases::getInstance().sLevel// Уровень
	 CString sRMS = TranslateString(L"СКЗ");
#define g_sRMS Phrases::getInstance().sRMS// СКЗ
	 CString sDelayBeforeMeasurement = TranslateString(L"Задержка перед измерениями");
#define g_sDelayBeforeMeasurement Phrases::getInstance().sDelayBeforeMeasurement// Задержка перед измерениями
	 CString sRecordingDuration = TranslateString(L"Длительность записи");
#define g_sRecordingDuration Phrases::getInstance().sRecordingDuration// Длительность записи
	 CString sRecordModeParameters = TranslateString(L"Параметры режима записи");
#define g_sRecordModeParameters Phrases::getInstance().sRecordModeParameters// Параметры режима записи
	 CString sRecordingParameters = TranslateString(L"Записываемые параметры");
#define g_sRecordingParameters Phrases::getInstance().sRecordingParameters// Записываемые параметры
	 CString sReplacingOlderFiles = TranslateString(L"Замена старых файлов");
#define g_sReplacingOlderFiles Phrases::getInstance().sReplacingOlderFiles// Замена старых файлов
	 CString sWithoutSplit = TranslateString(L"без разбивки");
#define g_sWithoutSplit Phrases::getInstance().sWithoutSplit// без разбивки
	 CString sBreakdownFiles = TranslateString(L"Разбивка файлов");
#define g_sBreakdownFiles Phrases::getInstance().sBreakdownFiles// Разбивка файлов
	 CString sSplitByDayssmall = TranslateString(L"разбивка по суткам");
#define g_sSplitByDayssmall Phrases::getInstance().sSplitByDayssmall// разбивка по суткам
	 CString sSplitByHoursssmall = TranslateString(L"разбивка по часу");
#define g_sSplitByHoursssmall Phrases::getInstance().sSplitByHoursssmall// разбивка по часу
	 CString sUse = TranslateString(L"Использовать");
#define g_sUse Phrases::getInstance().sUse// Использовать
	 CString sTensoBridge = TranslateString(L"¼-мостовая схема");
#define g_sTensoBridge Phrases::getInstance().sTensoBridge// ¼-мостовая схема
	 CString sTensoBridgeSettings = TranslateString(L"Настройка параметров ¼-мостовой схемы");
#define g_sTensoBridgeSettings Phrases::getInstance().sTensoBridgeSettings// Настройка параметров ¼-мостовой схемы
	 CString sUseDeviceReplaceOldOne = TranslateString(L"Использовать устройство взамен старого");
#define g_sUseDeviceReplaceOldOne Phrases::getInstance().sUseDeviceReplaceOldOne// Использовать устройство взамен старого
	 CString sDeleteFile = TranslateString(L"Удалить файл");
#define g_sDeleteFile Phrases::getInstance().sDeleteFile// Удалить файл
	 CString sReallyWanDeleteFiles = TranslateString(L"Вы действительно хотите удалить файлы?");
#define g_sReallyWanDeleteFiles Phrases::getInstance().sReallyWanDeleteFiles// Вы действительно хотите удалить файлы?
	 CString sDeleteConfig = TranslateString(L"Удалить конфигурацию");
#define g_sDeleteConfig Phrases::getInstance().sDeleteConfig// Удалить конфигурацию
	 CString sTotalStatusRecording = TranslateString(L"Общее состояние записи");
#define g_sTotalStatusRecording Phrases::getInstance().sTotalStatusRecording// Общее состояние записи
	 CString sRecordingStatusCurrentFile = TranslateString(L"Состояние записи текущего файла");
#define g_sRecordingStatusCurrentFile Phrases::getInstance().sRecordingStatusCurrentFile// Состояние записи текущего файла
	 CString sHigher = TranslateString(L"Выше");
#define g_sHigher Phrases::getInstance().sHigher// Выше
	 CString sLower = TranslateString(L"Ниже");
#define g_sLower Phrases::getInstance().sLower// Ниже
	 CString sApplySettingsOtherDevice = TranslateString(L"Применить настройки другого устройства.");
#define g_sApplySettingsOtherDevice Phrases::getInstance().sApplySettingsOtherDevice// Применить настройки другого устройства.
	 CString sTensoSettings = TranslateString(L"Тензо настройки");
#define g_sTensoSettings Phrases::getInstance().sTensoSettings// Тензо настройки
	 CString sWrongStartTimeSet = TranslateString(L"Выставлено неправильное время запуска!");
#define g_sWrongStartTimeSet Phrases::getInstance().sWrongStartTimeSet// Выставлено неправильное время запуска!
	 CString sEnteredNegativeRecordTime = TranslateString(L"Вы ввели отрицательное время записи!");
#define g_sEnteredNegativeRecordTime Phrases::getInstance().sEnteredNegativeRecordTime// Вы ввели отрицательное время записи!
	 CString sEnteredTooMuchRecordTime = TranslateString(L"Вы ввели слишком большое время записи!");
#define g_sEnteredTooMuchRecordTime Phrases::getInstance().sEnteredTooMuchRecordTime// Вы ввели слишком большое время записи!
	 CString sAddedStartTimeRegistration = TranslateString(L"Добавлено время старта регистрации");
#define g_sAddedStartTimeRegistration Phrases::getInstance().sAddedStartTimeRegistration//Добавлено время старта регистрации
	 CString sRunningThroughDigitalPort = TranslateString(L"Запуск через цифровой порт");
#define g_sRunningThroughDigitalPort Phrases::getInstance().sRunningThroughDigitalPort//Запуск через цифровой порт
	 CString sRecordingTimeNotLess1s = TranslateString(L"Время записи не может быть меньше одной секунды");
#define g_sRecordingTimeNotLess1s Phrases::getInstance().sRecordingTimeNotLess1s//Время записи не может быть меньше одной секунды
	 CString sSetRecordingTime1s = TranslateString(L"Установлено время записи в 1 секунду");
#define g_sSetRecordingTime1s Phrases::getInstance().sSetRecordingTime1s//Установлено время записи в 1 секунду
	 CString sIncorrectRecordingTime = TranslateString(L"Некорректное время записи");
#define g_sIncorrectRecordingTime Phrases::getInstance().sIncorrectRecordingTime//Некорректное время записи
	 CString sSetTimeRecord = TranslateString(L"Установить время записи");
#define g_sSetTimeRecord Phrases::getInstance().sSetTimeRecord//Установить время записи
	 CString sMaximumDurationEntry = TranslateString(L"Максимальная длительность записи при заданных параметрах устройства составляет");
#define g_sMaximumDurationEntry Phrases::getInstance().sMaximumDurationEntry//Максимальная длительность записи при заданных параметрах устройства составляет
	 CString sIncreaseTotalRecordingTime = TranslateString(L"Для увеличения общего времени записи освободите память в устройстве");
#define g_sIncreaseTotalRecordingTime Phrases::getInstance().sIncreaseTotalRecordingTime//Для увеличения общего времени записи освободите память в устройстве
	 CString sMaximumNumberRecordsSettings = TranslateString(L"Максимальное количество записей при заданных настройках");
#define g_sMaximumNumberRecordsSettings Phrases::getInstance().sMaximumNumberRecordsSettings//Максимальное количество записей при заданных настройках
	 CString sExceededMaximumValue = TranslateString(L"превысила максимальное значение");
#define g_sExceededMaximumValue Phrases::getInstance().sExceededMaximumValue//превысила максимальное значение
	 CString sTimeTriggeringListCleared = TranslateString(L"Очищен список запусков по времени");
#define g_sTimeTriggeringListCleared Phrases::getInstance().sTimeTriggeringListCleared//Очищен список запусков по времени
	 CString sParamWarning = TranslateString(L"Вы ввели некорректное наименование датчика. Имя датчика должно содержать от 1 до 32 символов.");
#define g_sParamWarning Phrases::getInstance().sParamWarning//Вы ввели некорректное наименование датчика. Имя датчика должно содержать от 1 до 32 символов.  
	 CString sIncorrectSimbol = TranslateString(L"Некорректный символ");
#define g_sIncorrectSimbol Phrases::getInstance().sIncorrectSimbol//Некорректный символ
	 CString sMaximumRecordingTime = TranslateString(L"Максимальное время записи");
#define g_sMaximumRecordingTime Phrases::getInstance().sMaximumRecordingTime//Максимальное время записи
	 CString sThresholdExceeded = TranslateString(L"Превышен порог");
#define g_sThresholdExceeded Phrases::getInstance().sThresholdExceeded//Превышен порог
	 CString sTotalSamplingFrequency = TranslateString(L"Превышен порог суммарной частоты дискретизации! Максимальная частота дискретизации");
#define g_sTotalSamplingFrequency Phrases::getInstance().sTotalSamplingFrequency//Превышен порог суммарной частоты дискретизации! Максимальная частота дискретизации
	 CString sSummSamplingFrequency = TranslateString(L"Суммарная частота дискретизации по всем каналам");
#define g_sSummSamplingFrequency Phrases::getInstance().sSummSamplingFrequency//Суммарная частота дискретизации по всем каналам
	 CString sMaximumLevel = TranslateString(L"максимальный уровень");
#define g_sMaximumLevel Phrases::getInstance().sMaximumLevel//максимальный уровень
	 CString sFrequencyEachChannel = TranslateString(L"Частота по каждому каналу");
#define g_sFrequencyEachChannel Phrases::getInstance().sFrequencyEachChannel//Частота по каждому каналу

	 CString sThresholdExceededWithGenerator = TranslateString(L"Превышен порог суммарной частоты дискретизации!");
#define g_sThresholdExceededWithGenerator Phrases::getInstance().sThresholdExceededWithGenerator//Превышен порог суммарной частоты дискретизации!
	 CString sOFFChannel = TranslateString(L"Отключите канал или встроенный генератор");
#define g_sOFFChannel Phrases::getInstance().sOFFChannel//Отключите канал или встроенный генератор
	 
	 // Настройки автономного регистратора ZSP
	 CString s_zsp_record_start_mode = TranslateString(L"Тип запуска регистрации сигналов");
#define g_s_zsp_record_start_mode Phrases::getInstance().s_zsp_record_start_mode	// Тип запуска регистрации сигналов
	 CString s_zsp_record_start_mode_button = TranslateString(L"По нажатию кнопки \"Запись\"");
#define g_s_zsp_record_start_mode_button Phrases::getInstance().s_zsp_record_start_mode_button	// По нажатию кнопки \"Запись\"
	 CString s_zsp_record_start_mode_auto = TranslateString(L"При подаче питания");
#define g_s_zsp_record_start_mode_auto Phrases::getInstance().s_zsp_record_start_mode_auto	// При подаче питания
	 CString s_zsp_record_continuous = TranslateString(L"Непрерывная запись");
#define g_s_zsp_record_continuous Phrases::getInstance().s_zsp_record_continuous	// Непрерывная запись
	 CString s_zsp_record_duration = TranslateString(L"Длительность записи, мин");
#define g_s_zsp_record_duration Phrases::getInstance().s_zsp_record_duration		// Длительность записи, мин

	 // Настройки самоконтроля
	 CString s_selftest_list = TranslateString(L"Список тестов");
#define g_s_selftest_list Phrases::getInstance().s_selftest_list	// Список тестов
	 
	 // Настройки синхронизации

	 CString sAllowDeviceMaster = TranslateString(L"Разрешить устройству быть ведущим");
#define g_sAllowDeviceMaster Phrases::getInstance().sAllowDeviceMaster//Разрешить устройству быть ведущим
	 CString sАllowDeviceSlave = TranslateString(L"Разрешить устройству быть ведомым");
#define g_sАllowDeviceSlave Phrases::getInstance().sАllowDeviceSlave//Разрешить устройству быть ведомым
	 CString sGreenPPSYes = TranslateString(L"Зеленый - PPS есть");
#define g_sGreenPPSYes Phrases::getInstance().sGreenPPSYes//Зеленый - PPS есть
	 CString sGreyPPSNo = TranslateString(L"Серый - PPS отсутствует");
#define g_sGreyPPSNo Phrases::getInstance().sGreyPPSNo//Серый - PPS отсутствует
	 CString sVirtualSubnetNumber = TranslateString(L"Номер виртуальной подсети");
#define g_sVirtualSubnetNumber Phrases::getInstance().sVirtualSubnetNumber//Номер виртуальной подсети
	 CString sPriorityMasterDevice = TranslateString(L"Приоритет ведущего устройства");
#define g_sPriorityMasterDevice Phrases::getInstance().sPriorityMasterDevice//Приоритет ведущего устройства
	 CString sAppliesAbsolutePriorities = TranslateString(L"Применяется в случае совпадения абсолютных приоритетов");
#define g_sAppliesAbsolutePriorities Phrases::getInstance().sAppliesAbsolutePriorities//Применяется в случае совпадения абсолютных приоритетов
	 CString sDeleteSelectedFiles = TranslateString(L"Удалить выбранные файлы?");
#define g_sDeleteSelectedFiles Phrases::getInstance().sDeleteSelectedFiles//Удалить выбранные файлы?
	 CString sDeleteSelectedFile = TranslateString(L"Удалить выбранный файл?");
#define g_sDeleteSelectedFile Phrases::getInstance().sDeleteSelectedFile//Удалить выбранный файл?
	 CString sAdjustmentLevel = TranslateString(L"Уровень подстройки");
#define g_sAdjustmentLevel Phrases::getInstance().sAdjustmentLevel//Уровень подстройки
	 CString sSynchronizationDigitalPort = TranslateString(L"Синхронизация по цифровому порту");
#define g_sSynchronizationDigitalPort Phrases::getInstance().sSynchronizationDigitalPort//Синхронизация по цифровому порту
	 CString sMaster = TranslateString(L"Ведущий");
#define g_sMaster Phrases::getInstance().sMaster//Ведущий
	 CString sSlave = TranslateString(L"Ведомый");
#define g_sSlave Phrases::getInstance().sSlave//Ведомый
	 CString sDeviceTime = TranslateString(L"Время устройства");
#define g_sDeviceTime Phrases::getInstance().sDeviceTime//Время устройства
	 CString sSynchronized = TranslateString(L"Синхронизировано");
#define g_sSynchronized Phrases::getInstance().sSynchronized//Синхронизировано
	 CString sDomain = TranslateString(L"Домен");
#define g_sDomain Phrases::getInstance().sDomain//Домен
	 CString sAbsolutePriority = TranslateString(L"Абсолютный приоритет");
#define g_sAbsolutePriority Phrases::getInstance().sAbsolutePriority//Абсолютный приоритет
	 CString sRelativePriority = TranslateString(L"Относительный приоритет");
#define g_sRelativePriority Phrases::getInstance().sRelativePriority//Относительный приоритет
	 CString sNOTELowerLalueHigherPriority = TranslateString(L"ПРИМ. Меньшее значение - больший приоритет");
#define g_sNOTELowerLalueHigherPriority Phrases::getInstance().sNOTELowerLalueHigherPriority//ПРИМ. Меньшее значение - больший приоритет
	 CString sPresenceSignal = TranslateString(L"Наличие сигнала");
#define g_sPresenceSignal Phrases::getInstance().sPresenceSignal//Наличие сигнала




//Состояние флеш-накопителя
	 CString sErrorOccurredReadingFileSD = TranslateString(L"Произошла ошибка чтения файла с карты SD-flash");
#define g_sErrorOccurredReadingFileSD Phrases::getInstance().sErrorOccurredReadingFileSD//Произошла ошибка чтения файла с карты SD-flash
	 CString sStatusFlashDrive = TranslateString(L"Состояние флеш-накопителя");
#define g_sStatusFlashDrive Phrases::getInstance().sStatusFlashDrive//Состояние флеш-накопителя
	 CString sErrorWorkingSD = TranslateString(L"Ошибка работы с картой SD-flash");
#define g_sErrorWorkingSD Phrases::getInstance().sErrorWorkingSD//Ошибка работы с картой SD-flash
	 CString sFlashDrive = TranslateString(L"Flash-накопитель");
#define g_sFlashDrive Phrases::getInstance().sFlashDrive//Flash-накопитель
	 CString sLocalDisk = TranslateString(L"Локальный диск");
#define g_sLocalDisk Phrases::getInstance().sLocalDisk//Локальный диск

//Настройки фильтров
	 CString sInputType = TranslateString(L"Тип входа");
#define g_sInputType Phrases::getInstance().sInputType//Тип входа
	 CString sAc = TranslateString(L"AC");
#define g_sAc Phrases::getInstance().sAc//AC
	 CString sDc = TranslateString(L"DC");
#define g_sDc Phrases::getInstance().sDc//DC
	 CString sCharge = TranslateString(L"Усилитель заряда");
#define g_sCharge Phrases::getInstance().sCharge//усилитель заряда




	 CString sBridge = TranslateString(L"Мост");
#define g_sBridge Phrases::getInstance().sBridge//Bridge
	 CString sHalfBridge = TranslateString(L"Полумост");
#define g_sHalfBridge Phrases::getInstance().sHalfBridge//HalfBridge
	 CString sQuarterBridge = TranslateString(L"Четвертьмост");
#define g_sQuarterBridge Phrases::getInstance().sQuarterBridge//QuarterBridge 
	 CString sTenso = TranslateString(L"Схема подключения");
#define g_sTenso Phrases::getInstance().sTenso//tenso 





// простые фразы eдиниц измерений: 
	 CString sUNIT_KG = TranslateString(L"кг");
#define g_sUNIT_KG Phrases::getInstance().sUNIT_KG // L"кг"
	 CString sUNIT_T = TranslateString(L"т");
#define g_sUNIT_T Phrases::getInstance().sUNIT_T // L"т"
	 CString sUNIT_DEG = TranslateString(L"°");
#define g_sUNIT_DEG Phrases::getInstance().sUNIT_DEG // L"°"
	 CString sUNIT_A = TranslateString(L"А");
#define g_sUNIT_A Phrases::getInstance().sUNIT_A // L"А"
	 CString sUNIT_ATM = TranslateString(L"атм");
#define g_sUNIT_ATM Phrases::getInstance().sUNIT_ATM // L"атм"
	 CString sUNIT_CEL = TranslateString(L"°C");
#define g_sUNIT_CEL Phrases::getInstance().sUNIT_CEL // L"°C"
	 CString sUNIT_G = TranslateString(L"g");
#define g_sUNIT_G Phrases::getInstance().sUNIT_G // L"g"
	 CString sUNIT_H = TranslateString(L"Н");
#define g_sUNIT_H Phrases::getInstance().sUNIT_H // L"Н"
	 CString sUNIT_KH = TranslateString(L"кН");
#define g_sUNIT_KH Phrases::getInstance().sUNIT_KH // L"кН"
	 CString sUNIT_MH = TranslateString(L"МН");
#define g_sUNIT_MH Phrases::getInstance().sUNIT_MH // L"МН"
	 CString sUNIT_HM = TranslateString(L"Н·м");
#define g_sUNIT_HM Phrases::getInstance().sUNIT_HM // L"Н·м"
	 CString sUNIT_KHM = TranslateString(L"кН·м");
#define g_sUNIT_KHM Phrases::getInstance().sUNIT_KHM // L"кН·м"
	 CString sUNIT_KPA = TranslateString(L"кПа");
#define g_sUNIT_KPA Phrases::getInstance().sUNIT_KPA // L"кПа"
	 CString sUNIT_MA = TranslateString(L"мА");
#define g_sUNIT_MA Phrases::getInstance().sUNIT_MA // L"мА"
	 CString sUNIT_MM = TranslateString(L"мм");
#define g_sUNIT_MM Phrases::getInstance().sUNIT_MM // L"мм"
	 CString sUNIT_MKM = TranslateString(L"мкм");
#define g_sUNIT_MKM Phrases::getInstance().sUNIT_MKM // L"мкм"
	 CString sUNIT_MKMM = TranslateString(L"мкм/м");
#define g_sUNIT_MKMM Phrases::getInstance().sUNIT_MKMM // L"мкм/м"
	 CString sUNIT_MVV = TranslateString(L"мВ/В");
#define g_sUNIT_MVV Phrases::getInstance().sUNIT_MVV // L"мВ/В"
	 CString sUNIT_S = TranslateString(L"с");
#define g_sUNIT_S Phrases::getInstance().sUNIT_S // L"с"
	 CString sUNIT_MPA = TranslateString(L"МПа");
#define g_sUNIT_MPA Phrases::getInstance().sUNIT_MPA // L"МПа"
	 CString sUNIT_GPA = TranslateString(L"ГПа");
#define g_sUNIT_GPA Phrases::getInstance().sUNIT_GPA // L"ГПа"
	 CString sUNIT_KGSMM2 = TranslateString(L"кгс/мм²");
#define g_sUNIT_KGSMM2 Phrases::getInstance().sUNIT_KGSMM2 // L"кгс/мм²"
	 CString sUNIT_M = TranslateString(L"м");
#define g_sUNIT_M Phrases::getInstance().sUNIT_M // L"м"
	 CString sUNIT_MKV = TranslateString(L"мкВ");
#define g_sUNIT_MKV Phrases::getInstance().sUNIT_MKV // L"мкВ"
	 CString sUNIT_MV = TranslateString(L"мВ");
#define g_sUNIT_MV Phrases::getInstance().sUNIT_MV // L"мВ"
	 CString sUNIT_PA = TranslateString(L"Па");
#define g_sUNIT_PA Phrases::getInstance().sUNIT_PA // L"Па"
	 CString sUNIT_V = TranslateString(L"В");
#define g_sUNIT_V Phrases::getInstance().sUNIT_V // L"В"
	 CString sUNIT_Hz = TranslateString(L"Гц");
#define g_sUNIT_Hz Phrases::getInstance().sUNIT_Hz // L"Гц"
	 CString sUNIT_kHz = TranslateString(L"кГц");
#define g_sUNIT_kHz Phrases::getInstance().sUNIT_kHz // L"кГц"
	 CString sUNIT_MilleSec = TranslateString(L"мс");
#define g_sUNIT_MilleSec Phrases::getInstance().sUNIT_MilleSec // L"мс"
	 CString sUNIT_byte = TranslateString(L"байт");
#define g_sUNIT_byte Phrases::getInstance().sUNIT_byte // L"байт"
	 CString sUNIT_GB = TranslateString(L"ГБ");
#define g_sUNIT_GB Phrases::getInstance().sUNIT_GB // L"ГБ"
	 CString sUNIT_MB = TranslateString(L"МБайт");
#define g_sUNIT_MB Phrases::getInstance().sUNIT_MB // L"МБайт"
	 CString sUNIT_KB = TranslateString(L"кБ");
#define g_sUNIT_KB Phrases::getInstance().sUNIT_KB // L"кБ"
	 CString sUNIT_Mbps = TranslateString(L"Мбит/с");
#define g_sUNIT_Mbps Phrases::getInstance().sUNIT_Mbps // L"Мбит/с"
	 CString sUNIT_MINUTES = TranslateString(L"мин");
#define g_sUNIT_MINUTES Phrases::getInstance().sUNIT_MINUTES // L"мин"
	 CString sUNIT_HOURS = TranslateString(L"ч");
#define g_sUNIT_HOURS Phrases::getInstance().sUNIT_HOURS // L"ч"
	 CString sUNIT_YEAR = TranslateString(L"год");
#define g_sUNIT_YEAR Phrases::getInstance().sUNIT_YEAR // L"год"
	 CString sUNIT_MONTH = TranslateString(L"мес");
#define g_sUNIT_MONTH Phrases::getInstance().sUNIT_MONTH // L"мес"
	 CString sUNIT_YEARS = TranslateString(L"года");
#define g_sUNIT_YEARS Phrases::getInstance().sUNIT_YEARS // L"года"
	 CString sRECORDTIME = TranslateString(L"Время записи");
#define g_sRECORDTIME Phrases::getInstance().sRECORDTIME // L"Время записи"
	 CString sUNIT_LET = TranslateString(L"лет");
#define g_sUNIT_LET Phrases::getInstance().sUNIT_LET // L"лет"
	 CString sUNIT_DAY = TranslateString(L"дн");
#define g_sUNIT_DAY Phrases::getInstance().sUNIT_DAY // L"дн"	
	 CString sUNIT_dB = TranslateString(L"дБ");
#define g_sUNIT_dB Phrases::getInstance().sUNIT_dB // L"дБ"	
	 CString sUNIT_KBSec = TranslateString(L"КБ/с");
#define g_sUNIT_KBSec Phrases::getInstance().sUNIT_KBSec // L"КБ/с"	
	 CString sUNIT_KV = TranslateString(L"кВ");
#define g_sUNIT_KV Phrases::getInstance().sUNIT_KV // L"кВ"
	 CString sUNIT_OM = TranslateString(L"Ом");
#define g_sUNIT_OM Phrases::getInstance().sUNIT_OM // L"Ом"
	 CString sUNIT_kOM = TranslateString(L"кОм");
#define g_sUNIT_kOM Phrases::getInstance().sUNIT_kOM // L"кОм"
	 CString sUNIT_MOM = TranslateString(L"МОм");
#define g_sUNIT_MOM Phrases::getInstance().sUNIT_MOM // L"МОм"
	 CString sUNIT_MPERM = TranslateString(L"м/м");
#define g_sUNIT_MPERM Phrases::getInstance().sUNIT_MPERM // L"м/м"
	 CString sUNIT_MMPERM = TranslateString(L"мм/м");
#define g_sUNIT_MMPERM Phrases::getInstance().sUNIT_MMPERM // L"мм/м"
	 CString sUNIT_PERCENT = TranslateString(L"%");
#define g_sUNIT_PERCENT Phrases::getInstance().sUNIT_PERCENT // L"%"

	 // комбинированные фразы

	 CString sUNIT_G2HZ_1 = L"g²" + TranslateString(L"Гц") + L"⁻¹";
#define g_G2HZ_1 Phrases::getInstance().sUNIT_G2HZ_1 //L"g²Гц⁻¹"
	 CString sUNIT_2HZ_1 = L"²" + TranslateString(L"Гц") + L"⁻¹";
#define g_2HZ_1 Phrases::getInstance().sUNIT_2HZ_1 // L"²Гц⁻¹" 
	 CString sUNIT_HZ_SEC = TranslateString(L"Гц") + L"/" + TranslateString(L"с");
#define g_HZ_SEC Phrases::getInstance().sUNIT_HZ_SEC // L"Гц/с" 
	 CString sUNIT_MMPERS = TranslateString(L"мм") + L"/" + TranslateString(L"с");
#define g_sUNIT_MMPERS Phrases::getInstance().sUNIT_MMPERS  //L"мм/с"
	 CString sUNIT_MPERS = TranslateString(L"м") + L"/" + TranslateString(L"с");
#define g_sUNIT_MPERS Phrases::getInstance().sUNIT_MPERS  //L"м/с" 
	 CString sUNIT_MPERS2 = TranslateString(L"м") + L"/" + TranslateString(L"с") + L"²";
#define g_sUNIT_MPERS2 Phrases::getInstance().sUNIT_MPERS2  //  L"м/с²"

	 CString sGENERATOR = TranslateString(L"Встроенный генератор");
#define g_sGENERATOR Phrases::getInstance().sGENERATOR // L"Встроенный генератор"
	 CString sSINUS = TranslateString(L"Синус");
#define g_sSINUS Phrases::getInstance().sSINUS // L"Синус"
	 CString sGENERAL = TranslateString(L"Общие");
#define g_sGENERAL Phrases::getInstance().sGENERAL // L"Общие"
	 CString sEthernet = L"Ethernet";
#define g_sEthernet Phrases::getInstance().sEthernet // L"Ethernet"
	  CString sAUTOREGISTRATOR = TranslateString(L"Автономный регистратор");
#define g_sAUTOREGISTRATOR Phrases::getInstance().sAUTOREGISTRATOR // L"Автономный регистратор"
	 
	  CString sXXXX = L"7xxx";
#define g_sXXXX Phrases::getInstance().sXXXX // L"7xxx"

	  CString sPPS = L"PPS";
#define g_sPPS Phrases::getInstance().sPPS // L"PPS"

	  CString sNULLSIZECFG = TranslateString(L"Файл DEVICES.CFG имеет нулевой размер");
#define g_sNULLSIZECFG Phrases::getInstance().sNULLSIZECFG // Файл DEVICES.CFG имеет нулевой размер


	  CString sDAMAGEDEVICES = TranslateString(L"Файл DEVICES.CFG поврежден");
#define g_sDAMAGEDEVICES Phrases::getInstance().sDAMAGEDEVICES // Файл DEVICES.CFG поврежден

	  CString sChangeDEVICES = TranslateString(L"DEVICES.CFG был изменен");
#define g_sChangeDEVICES Phrases::getInstance().sChangeDEVICES // DEVICES.CFG был изменен

	  CString sChangeCFG = TranslateString(L"Исправить коллизию?");
#define g_sChangeCFG Phrases::getInstance().sChangeCFG // Исправить коллизию?

	  CString sCFGNotDonload = TranslateString(L"Конфигурационный файл не скачан");
#define g_sCFGNotDonload Phrases::getInstance().sCFGNotDonload // Конфигурационный файл не скачан

	  CString sContinue = TranslateString(L"Продолжить?");
#define g_sContinue Phrases::getInstance().sContinue // Продолжить?
	  
	 //extern CString g_IDS_IDD_REGISTRATOR_ZET048_TAB;		//L "Автономный регистратор"//L "Автономный регистратор" нельзя выносить, так как приводит к неправильной работе с ZET7000	

	  CString sTestRTC = TranslateString(L"Тест RTC");
#define g_sTestRTC Phrases::getInstance().sTestRTC // Тест RTC 
	 CString sRTCTestFailed = TranslateString(L"Тест RTC не пройден (не изменяется время RTC в устройстве)");
#define g_sRTCTestFailed Phrases::getInstance().sRTCTestFailed // Тест RTC не пройден (не изменяется время RTC в устройстве) 
	 CString sRTCTestFailedJumps = TranslateString(L"Тест RTC не пройден (при чтении времени RTC были скачки)");
#define g_sRTCTestFailedJumps Phrases::getInstance().sRTCTestFailedJumps // Тест RTC не пройден (при чтении времени RTC были скачки) 
	 CString sRTCTestPassed = TranslateString(L"Тест RTC пройден:");
#define g_sRTCTestPassed Phrases::getInstance().sRTCTestPassed // Тест RTC пройден: 
	 CString sValueSecond = TranslateString(L"значение секунды");
#define g_sValueSecond Phrases::getInstance().sValueSecond // значение секунды 
	 CString sStabilitySecond = TranslateString(L"стабильность секунды");
#define g_sStabilitySecond Phrases::getInstance().sStabilitySecond // стабильность секунды 
	 CString sDelay = TranslateString(L"отставание");
#define g_sDelay Phrases::getInstance().sDelay // отставание 
	 CString sTiming = TranslateString(L"опережение");
#define g_sTiming Phrases::getInstance().sTiming // опережение 
	 CString sRTCTestFailedSubtract = TranslateString(L"Тест RTC не пройден (не удалось вычитать время RTC)");
#define g_sRTCTestFailedSubtract Phrases::getInstance().sRTCTestFailedSubtract // Тест RTC не пройден (не удалось вычитать время RTC) 
	 CString sSuspender = TranslateString(L"Прервать");
#define g_sSuspender Phrases::getInstance().sSuspender // Прервать 
	 CString sDoSaveCurrentConfig = TranslateString(L"Вы хотите сохранить текущую конфигурацию оборудования?");
#define g_sDoSaveCurrentConfig Phrases::getInstance().sDoSaveCurrentConfig // Вы хотите сохранить текущую конфигурацию оборудования? 
	 CString sNameBusyAlready = TranslateString(L"Конфигурация с таким именем уже существует");
#define g_sNameBusyAlready Phrases::getInstance().sNameBusyAlready // Конфигурация с таким именем уже существует
	 CString sReWrite = TranslateString(L"Перезаписать?");
#define g_sReWrite Phrases::getInstance().sReWrite // Перезаписать?
	 CString sSaveConfig = TranslateString(L"Сохранить конфигурацию");
#define g_sSaveConfig Phrases::getInstance().sSaveConfig // Сохранить конфигурацию
	 CString sOpenConfig = TranslateString(L"Загрузить конфигурацию");
#define g_sOpenConfig Phrases::getInstance().sOpenConfig // Загрузить конфигурацию
	 CString sConfig = TranslateString(L"Конфигурация");
#define g_sConfig Phrases::getInstance().sConfig // Конфигурация
	 CString sDateTime = TranslateString(L"Дата/Время");
#define g_sDateTime Phrases::getInstance().sDateTime // Дата/Время

	 CString sLimitSdCard = TranslateString(L"На sd карте превышен лимит количества файлов");
#define g_sLimitSdCard Phrases::getInstance().sLimitSdCard // На sd карте превышен лимит количества файлов

	 CString sEmptyFilesForConv = TranslateString(L"Файлы для конвертации не найдены");
#define g_sEmptyFilesForConv Phrases::getInstance().sEmptyFilesForConv // Файлы для конвертации не найдены

	 CString sError = TranslateString(L"Ошибка");
#define g_sError Phrases::getInstance().sError // Ошибка
	 CString sCoordinate = TranslateString(L"Координаты");
#define g_sCoordinate Phrases::getInstance().sCoordinate //Координаты
	 CString sOffTEDS = TranslateString(L"Для использования TEDS отключите канал генератора");
#define g_sOffTEDS Phrases::getInstance().sOffTEDS // Для использования TEDS отключите канал генератора
	 CString sCheckChooseFolder = TranslateString(L"Проверьте наличие выбранной папки");
#define g_sCheckChooseFolder Phrases::getInstance().sCheckChooseFolder // Проверьте наличие выбранной папки
	 CString sOrientation = TranslateString(L"Ориентация");
#define g_sOrientation Phrases::getInstance().sOrientation // Ориентация
	 CString sCalibrationDevice = TranslateString(L"Устройство не откалибровано");
#define g_sCalibrationDevice Phrases::getInstance().sCalibrationDevice // Устройство не откалибровано
	 CString sInputResistance = TranslateString(L"Входное сопротивление");
#define g_sInputResistance Phrases::getInstance().sInputResistance // Входное сопротивление
	 CString sSelftestMode = TranslateString(L"Режим самотестирования");
#define g_sSelftestMode Phrases::getInstance().sSelftestMode // Режим самотестирования
	 CString sNoiseMode = TranslateString(L"Режим измерения собственных шумов");
#define g_sNoiseMode Phrases::getInstance().sNoiseMode // Режим измерения собственных шумов

	 //Tenso window фразы
	 CString sTensoWindow = TranslateString(L"Настройка параметров мостовой схемы");
#define g_sTensoWindow Phrases::getInstance().sTensoWindow // Настройка параметров мостовой схемы
	 CString Balance = TranslateString(L"Балансировка");
#define g_Balance Phrases::getInstance().Balance // Балансировка
	 CString sReferenceSensorResistance = TranslateString(L"Сопротивление опорного резистора");
#define g_sReferenceSensorResistance Phrases::getInstance().sReferenceSensorResistance // Сопротивление опорного резистора
	 CString sTenzoResistance = TranslateString(L"Сопротивление тензорезистора");
#define g_sTenzoResistance Phrases::getInstance().sTenzoResistance // Сопротивление тензорезистора
	 CString sAutoBalance = TranslateString(L"Автобалансировка");
#define g_sAutoBalance  Phrases::getInstance().sAutoBalance // Автобалансировка
	 CString sMeasure = TranslateString(L"Измерить");
#define g_sMeasure Phrases::getInstance().sMeasure // Измерить
	 
	 CString sTensoWarningMsg1 = TranslateString(L"Ошибка автобалансировки. Увеличьте сопротивление опорного резистора и выполните автобалансировку заново.");
#define g_sTensoWarningMsg1 Phrases::getInstance().sTensoWarningMsg1 // Ошибка автобалансировки. Увеличьте сопротивление опорного резистора и выполните автобалансировку заново.
	 CString sTensoWarningMsg2 = TranslateString(L"Ошибка автобалансировки. Уменьшите сопротивление опорного резистора и выполните автобалансировку заново.");
#define g_sTensoWarningMsg2 Phrases::getInstance().sTensoWarningMsg2 // Ошибка автобалансировки. Уменьшите сопротивление опорного резистора и выполните автобалансировку заново.
	 CString sTensoWarningMsg3 = TranslateString(L"Ошибка измерения сопротивления тензорезистора. Повторите автобалансировку.");
#define g_sTensoWarningMsg3 Phrases::getInstance().sTensoWarningMsg3 // Ошибка измерения сопротивления тензорезистора. Повторите автобалансировку.
	 CString sTensoWarningMsg4 = TranslateString(L"Ошибка измерения сопротивления тензорезистора. Проверьте тензорезистор.");
#define g_sTensoWarningMsg4 Phrases::getInstance().sTensoWarningMsg4 // Ошибка измерения сопротивления тензорезистора. Проверьте тензорезистор.
	 CString sTensoWarningMsg5 = TranslateString(L"Ошибка измерения сопротивления тензорезистора. Уменьшите коэффициент усиления по каналу и выполните автобалансировку заново.");
#define g_sTensoWarningMsg5 Phrases::getInstance().sTensoWarningMsg5 // Ошибка измерения сопротивления тензорезистора. Уменьшите коэффициент усиления по каналу и выполните автобалансировку заново.
	 CString sTensoWarningMsg6 = TranslateString(L"Ошибка измерения сопротивления тензорезистора. Уменьшите сопротивление опорного резистора и выполните автобалансировку заново.");
#define g_sTensoWarningMsg6 Phrases::getInstance().sTensoWarningMsg6 // Ошибка измерения сопротивления тензорезистора. Уменьшите сопротивление опорного резистора и выполните автобалансировку заново.
	 CString sTensoWarningMsg7 = TranslateString(L"Ошибка измерения сопротивления тензорезистора. Увеличьте сопротивление опорного резистора и выполните автобалансировку заново.");
#define g_sTensoWarningMsg7 Phrases::getInstance().sTensoWarningMsg7 // Ошибка измерения сопротивления тензорезистора. Увеличьте сопротивление опорного резистора и выполните автобалансировку заново.
	 CString sTensoWarningMsg8 = TranslateString(L"Ошибка измерения сопротивления тензорезистора. Измеренное значение тензорезистора не входит диапазон допустимых значений.");
#define g_sTensoWarningMsg8 Phrases::getInstance().sTensoWarningMsg8 // Ошибка измерения сопротивления тензорезистора. Измеренное значение тензорезистора не входит диапазон допустимых значений.
	 CString sTensoWarningMsg9 = TranslateString(L"Превышен максимальный ток для выбранной схемы измерений. Установите напряжение питание не более чем %.3f В.");
#define g_sTensoWarningMsg9 Phrases::getInstance().sTensoWarningMsg9 // Превышен максимальный ток для выбранной схемы измерений. Установите напряжение питание не более чем %.3f В.
	 CString sTurnOnGeneratorTenso = TranslateString(L"Для тензоизмерений необходимо включить генератор для питания датчиков");
#define g_sTurnOnGeneratorTenso Phrases::getInstance().sTurnOnGeneratorTenso // Для тензоизмерений необходимо включить генератор для питания датчиков
	  CString sTensoWarning = TranslateString(L"Предупреждение");
#define g_sTensoWarning Phrases::getInstance().sTensoWarning // Предупреждение
	 CString sPropGenerator = TranslateString(L"Свойства генератора");
#define g_sPropGenerator Phrases::getInstance().sPropGenerator // Свойства генератора
	 CString sSetZeroShift = TranslateString(L"Выполнить смещение в 0?");
#define g_sSetZeroShift Phrases::getInstance().sSetZeroShift // Выполнить смещение в 0?

	 CString sDCSost = TranslateString(L"Пост. сост.");
#define g_sDCSost Phrases::getInstance().sDCSost // Пост. сост.
	 CString sACSost = TranslateString(L"Переменн. сост.");
#define g_sACSost Phrases::getInstance().sACSost // Переменн. сост.
	 CString sIdentificatorBusy = TranslateString(L"Идентификатор занят");
#define g_sIdentificatorBusy Phrases::getInstance().sIdentificatorBusy // Идентификатор занят
	 CString sForProgrammableComponents = TranslateString(L"Для подключения устройства вам необходима лицензия. Подключите ZETKey");
#define g_sForProgrammableComponents Phrases::getInstance().sForProgrammableComponents // Для подключения устройства вам необходима лицензия. Подключите ZETKey
	 CString sNetWizard = TranslateString(L"Подключение устройств по Ethernet");
#define g_sNetWizard Phrases::getInstance().sNetWizard // Подключение устройств по Ethernet
	 CString sDetectedZet76 = TranslateString(L"В сети обнаружены устройства ZETSENSOR") + L".\n\n" + TranslateString(L"Будет запущена программа для настройки подключения к этим устройствам") + L".";
#define g_sDetectedZet76 Phrases::getInstance().sDetectedZet76 // В локальной сети обнаружены устройства ZETSENSOR. Запустить программу для их подключения?
	 CString sSlot = TranslateString(L"модуль");
#define g_sSlot Phrases::getInstance().sSlot // модуль

	 CString sIScheme = TranslateString(L"Измерение тока");
#define g_sIScheme Phrases::getInstance().sIScheme // Измерение тока
	 CString sUScheme = TranslateString(L"Измерение напряжения");
#define g_sUScheme Phrases::getInstance().sUScheme // Измерение напряжения
	 CString sTrScheme = TranslateString(L"Измерение температуры");
#define g_sTScheme Phrases::getInstance().sTrScheme // Измерение температуры
	 CString sLvdtScheme = TranslateString(L"Питание переменным током");
#define g_sLvdtScheme Phrases::getInstance().sLvdtScheme // Питание переменным током
	 CString sFullBridge4wire = TranslateString(L"Мостовая (четырехпроводная)");
#define g_sFullBridge4wire Phrases::getInstance().sFullBridge4wire // Мостовая (четырехпроводная)
	 CString sFullBridge4wireU = TranslateString(L"Мостовая по напр. (четырехпроводная)");
#define g_sFullBridge4wireU Phrases::getInstance().sFullBridge4wireU // Мостовая по напр. (четырехпроводная)
	 CString sFullBridge4wireI = TranslateString(L"Мостовая по току (четырехпроводная)");
#define g_sFullBridge4wireI Phrases::getInstance().sFullBridge4wireI // Мостовая по току (четырехпроводная)
	 CString sFullBridge6wire = TranslateString(L"Мостовая (шестипроводная)");
#define g_sFullBridge6wire Phrases::getInstance().sFullBridge6wire // Мостовая (шестипроводная)
	 CString sHalfBridge3wire = TranslateString(L"½-мостовая (трехпроводная)");
#define g_sHalfBridge3wire Phrases::getInstance().sHalfBridge3wire // ½-мостовая (трехпроводная)
	 CString sHalfBridge5wire = TranslateString(L"½-мостовая (пятипроводная)");
#define g_sHalfBridge5wire Phrases::getInstance().sHalfBridge5wire // ½-мостовая (пятипроводная)
	 CString sQuaterBridgeR1 = TranslateString(L"¼-мостовая (R = 100 Ом)");
#define g_sQuaterBridgeR1 Phrases::getInstance().sQuaterBridgeR1 // ¼-мостовая (R = 100 Ом)
	 CString sQuaterBridgeR2 = TranslateString(L"¼-мостовая (R = 120 Ом)");
#define g_sQuaterBridgeR2 Phrases::getInstance().sQuaterBridgeR2 // ¼-мостовая (R = 120 Ом)
	 CString sQuaterBridgeR3 = TranslateString(L"¼-мостовая (R = 350 Ом)");
#define g_sQuaterBridgeR3 Phrases::getInstance().sQuaterBridgeR3 // ¼-мостовая (R = 350 Ом)
	 CString sQuaterBridgeR4 = TranslateString(L"¼-мостовая (R = 400 Ом)");
#define g_sQuaterBridgeR4 Phrases::getInstance().sQuaterBridgeR4 // ¼-мостовая (R = 400 Ом)
	 CString sTensoScheme = TranslateString(L"Схема измерения");
#define g_sTensoScheme Phrases::getInstance().sTensoScheme // Схема измерения
	 CString sTensoExc = TranslateString(L"Ток питания схемы, мА");
#define g_sTensoExc Phrases::getInstance().sTensoExc // Ток питания схемы, мА
	 CString sTensoResistance = TranslateString(L"Сопротивление датчика, Ом");
#define g_sTensoResistance Phrases::getInstance().sTensoResistance // Сопротивление датчика, Ом
	 CString sNsh = TranslateString(L"Ном. стат. характеристика (НСХ)");
#define g_sNsh Phrases::getInstance().sNsh // Ном. стат. характеристика (НСХ)
	 CString sTcType = TranslateString(L"Тип термопары");
#define g_sTcType Phrases::getInstance().sTcType // Тип термопары
	 CString sTensoResistor = TranslateString(L"Тензорезистор");
#define g_sTensoResistor Phrases::getInstance().sTensoResistor // Тензорезистор
	 CString sTensoSensor = TranslateString(L"Тензодатчик");
#define g_sTensoSensor Phrases::getInstance().sTensoSensor // Тензодатчик
	 CString sLvdt = TranslateString(L"LVDT-датчик");
#define g_sLvdt Phrases::getInstance().sLvdt // LVDT-датчик
	 CString sTr = TranslateString(L"Термосопротивление");
#define g_sTr Phrases::getInstance().sTr // Термосопротивление
	 CString sTc = TranslateString(L"Термопара");
#define g_sTc Phrases::getInstance().sTc // Термопара
	 CString sDiff = TranslateString(L"Датчик с выходом по напряжению (дифф.)");
#define g_sDiff Phrases::getInstance().sDiff // Датчик с выходом по напряжению (дифф.)
	 CString sSingle = TranslateString(L"Датчик с выходом по напряжению (синф.)");
#define g_sSingle Phrases::getInstance().sSingle // Датчик с выходом по напряжению (синф.)
	 CString sCurrent = TranslateString(L"Датчик с выходом по току");
#define g_sCurrent Phrases::getInstance().sCurrent // Датчик с выходом по току
	 CString sTensoUser = TranslateString(L"Пользовательский");
#define g_sTensoUser Phrases::getInstance().sTensoUser // Пользовательский
	 CString sTensoSensorType = TranslateString(L"Тип датчика");
#define g_sTensoSensorType Phrases::getInstance().sTensoSensorType // Тип датчика
	 CString sTensionCompression = TranslateString(L"Растяжение / Сжатие");
#define g_sTensionCompression Phrases::getInstance().sTensionCompression // Растяжение/Сжатие
	 CString sBending = TranslateString(L"Изгиб");
#define g_sBending Phrases::getInstance().sBending // Изгиб
	 CString sTorsion = TranslateString(L"Кручение");
#define g_sTorsion Phrases::getInstance().sTorsion // Кручение

	 CString sNsh_100p = TranslateString(L"100П");
#define g_sNsh_100p Phrases::getInstance().sNsh_100p // 100П
	 CString sNsh_50p = TranslateString(L"50П");
#define g_sNsh_50p Phrases::getInstance().sNsh_50p // 50П
	 CString sNsh_pt1000 = TranslateString(L"Pt1000");
#define g_sNsh_pt1000 Phrases::getInstance().sNsh_pt1000 // Pt1000
	 CString sNsh_pt100 = TranslateString(L"Pt100");
#define g_sNsh_pt100 Phrases::getInstance().sNsh_pt100 // Pt100
	 CString sNsh_pt50 = TranslateString(L"Pt50");
#define g_sNsh_pt50 Phrases::getInstance().sNsh_pt50 // Pt50
	 CString sNsh_100m = TranslateString(L"100М");
#define g_sNsh_100m Phrases::getInstance().sNsh_100m // 100М
	 CString sNsh_50m = TranslateString(L"50М");
#define g_sNsh_50m Phrases::getInstance().sNsh_50m // 50М
	 CString sNsh_cu100 = TranslateString(L"Cu100");
#define g_sNsh_cu100 Phrases::getInstance().sNsh_cu100 // Cu100
	 CString sNsh_cu50 = TranslateString(L"Cu50");
#define g_sNsh_cu50 Phrases::getInstance().sNsh_cu50 // Cu50
	 CString sNsh_100n = TranslateString(L"100Н");
#define g_sNsh_100n Phrases::getInstance().sNsh_100n // 100Н
	 CString sNsh_50n = TranslateString(L"50Н");
#define g_sNsh_50n Phrases::getInstance().sNsh_50n // 50Н

	 CString sType = TranslateString(L"Тип");
#define g_sType Phrases::getInstance().sType // Тип
	
	 CString s_network = TranslateString(L"Код сети станций (2 символа)");
#define g_s_network Phrases::getInstance().s_network // Код сети станций (2 символа)
	 CString s_station = TranslateString(L"Код станции (5 символов)");
#define g_s_station Phrases::getInstance().s_station // Код станции (5 символов)
	 CString s_latitude = TranslateString(L"Географическая широта, °");
#define g_s_latitude Phrases::getInstance().s_latitude // Географическая широта, °
	 CString s_longitude = TranslateString(L"Географическая долгота, °");
#define g_s_longitude Phrases::getInstance().s_longitude // Географическая долгота, °
	 CString s_elevation = TranslateString(L"Высота над уровнем моря, м");
#define g_s_elevation Phrases::getInstance().s_elevation // Высота над уровнем моря, м
	 CString s_site_name = TranslateString(L"Описание расположения");
#define g_s_site_name Phrases::getInstance().s_site_name // Описание расположения
	 
};
//----------------------------------------------------------------------------------------------------------------------
typedef CSingleton<PhrasesImpl> Phrases;