#ifndef UART_3_H_
#define UART_3_H_

class UART_3_Class
{
private:


public:
    void Init_UART(uint32_t baud, uint32_t F_CPU_Hz);
    void Clear_Screen();
    
    /*
    * \def UART3_Write
    * \brief Writes a character to UART3
    * \param data (character to send)
    */
    void Write_Char(char data);
    
    /*
    * \def Write_String
    * \brief Writes a string to UART3
    * \param data (text to send)
    */
    void Write_String(char *text);
        
};
#endif
