/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h> // For memset
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NAND_PAGE_SIZE         4224 // 4096 + 128 spare
#define NAND_BLOCK_SIZE        (NAND_PAGE_SIZE * 64) // 64 pages per block
#define NAND_TOTAL_BLOCKS      4096
#define NAND_COMMAND_ADDRESS   ((uint32_t)0x80000000) // FMC Command address
#define NAND_DATA_ADDRESS      ((uint32_t)0x80001000) // FMC Data address
#define CLE                    0x00008000
#define ALE                    0x00004000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
NAND_HandleTypeDef hnand1;

/* USER CODE BEGIN PV */
uint8_t write_data[NAND_PAGE_SIZE];
uint8_t read_data[NAND_PAGE_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FMC_Init(void);
void NAND_SendCommand(uint8_t command);
void NAND_SendAddress(uint8_t address);
void NAND_WriteData(uint8_t *data, uint32_t length);
void NAND_ReadData(uint8_t *data, uint32_t length);
void NAND_WritePage(uint8_t *data, uint32_t page_address);
void NAND_ReadPage(uint8_t *data, uint32_t page_address);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FMC_Init();

  /* USER CODE BEGIN 2 */
  // Initialize test data
  memset(write_data, 0xAA, NAND_PAGE_SIZE); // Fill with pattern 0xAA
  memset(read_data, 0x00, NAND_PAGE_SIZE);  // Clear read buffer

  // Write data to page 0
  NAND_WritePage(write_data, 0);

  // Read data back from page 0
  NAND_ReadPage(read_data, 0);

  // Verify data
  if (memcmp(write_data, read_data, NAND_PAGE_SIZE) == 0)
  {
    // Data matched, indicate success (e.g., LED ON)
  }
  else
  {
    // Data mismatch, indicate failure (e.g., LED OFF or Error Handler)
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  // Clock configuration as per your setup
}

/* FMC initialization function */
static void MX_FMC_Init(void)
{
  FMC_NAND_PCC_TimingTypeDef ComSpaceTiming = {0};
  FMC_NAND_PCC_TimingTypeDef AttSpaceTiming = {0};

  hnand1.Instance = FMC_NAND_DEVICE;
  hnand1.Init.NandBank = FMC_NAND_BANK3;
  hnand1.Init.Waitfeature = FMC_NAND_WAIT_FEATURE_ENABLE;
  hnand1.Init.MemoryDataWidth = FMC_NAND_MEM_BUS_WIDTH_8;
  hnand1.Init.EccComputation = FMC_NAND_ECC_ENABLE;
  hnand1.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_512BYTE;
  hnand1.Init.TCLRSetupTime = 1;
  hnand1.Init.TARSetupTime = 1;

  hnand1.Config.PageSize = NAND_PAGE_SIZE;
  hnand1.Config.SpareAreaSize = 128;
  hnand1.Config.BlockSize = 64;
  hnand1.Config.BlockNbr = NAND_TOTAL_BLOCKS;
  hnand1.Config.PlaneNbr = 1;
  hnand1.Config.PlaneSize = 4096;
  hnand1.Config.ExtraCommandEnable = DISABLE;

  ComSpaceTiming.SetupTime = 12;
  ComSpaceTiming.WaitSetupTime = 20;
  ComSpaceTiming.HoldSetupTime = 10;
  ComSpaceTiming.HiZSetupTime = 10;

  AttSpaceTiming = ComSpaceTiming;

  if (HAL_NAND_Init(&hnand1, &ComSpaceTiming, &AttSpaceTiming) != HAL_OK)
  {
    Error_Handler();
  }
}

/* GPIO initialization function */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
}

/* NAND command and data handling functions */
void NAND_SendCommand(uint8_t command)
{
  *(__IO uint8_t *)(NAND_COMMAND_ADDRESS | CLE) = command;
}

void NAND_SendAddress(uint8_t address)
{
  *(__IO uint8_t *)(NAND_COMMAND_ADDRESS | ALE) = address;
}

void NAND_WriteData(uint8_t *data, uint32_t length)
{
  for (uint32_t i = 0; i < length; i++)
  {
    *(__IO uint8_t *)NAND_DATA_ADDRESS = data[i];
  }
}

void NAND_ReadData(uint8_t *data, uint32_t length)
{
  for (uint32_t i = 0; i < length; i++)
  {
    data[i] = *(__IO uint8_t *)NAND_DATA_ADDRESS;
  }
}

void NAND_WritePage(uint8_t *data, uint32_t page_address)
{
  NAND_SendCommand(0x80); // Program page command
  NAND_SendAddress(page_address & 0xFF);
  NAND_SendAddress((page_address >> 8) & 0xFF);
  NAND_SendAddress((page_address >> 16) & 0xFF);
  NAND_WriteData(data, NAND_PAGE_SIZE);
  NAND_SendCommand(0x10); // Program confirm
  HAL_Delay(1); // Wait for programming to complete
}

void NAND_ReadPage(uint8_t *data, uint32_t page_address)
{
  NAND_SendCommand(0x00); // Read command
  NAND_SendAddress(page_address & 0xFF);
  NAND_SendAddress((page_address >> 8) & 0xFF);
  NAND_SendAddress((page_address >> 16) & 0xFF);
  NAND_SendCommand(0x30); // Read confirm
  HAL_Delay(1); // Wait for data to be ready
  NAND_ReadData(data, NAND_PAGE_SIZE);
}

/**
  * @brief Error Handler
  */
void Error_Handler(void)
{
  while (1)
  {
    // Stay here to debug
  }
}
