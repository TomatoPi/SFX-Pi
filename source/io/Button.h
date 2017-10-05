#ifndef DEF_BUTTON_H
#define DEF_BUTTON_H

typedef enum{
    
    PUSH,
    PULL,
    PUSH_PUSH,
    MOMENT
}IO_PUSH_TYPE;

typedef enum{

    FUNC_MENU,
    FUNC_BANK_NEXT,
    FUNC_BANK_PREV
}IO_BUTTON_FUNCTION;

/**
 * Object containning all datas for adress a button or an io
 * It contain on wich register the adress refers to
 * On wich register the adress is
 * The hex value of the adress
 */
class IO_Adress{

    public :
    
        /**
         * Adress Constructor
         */
        IO_Adress( unsigned char adr, bool gb, bool m1 ):
                    adr_(adr), gpiob_(gb), mcp1_(m1)
        {
        }

        unsigned char adr_; /**< Store Hex adress inside register */
        bool gpiob_;    /**< True if Adress is on gpioB */
        bool mcp1_;     /**< true if Adress is on MCP1 */
};

/**
 * Class used for store datas about buttons
 */
class IO_Button{

    public :

        /**
         * Button Constructor.
         * The button adress can only be set at button definition
         * @param type define if button is considered as a toggle
         *              or in momentary mode.
         * @param func define button function inside program
         * @param adr button adress on MCP registers
         * @see IO_PUSH_TYPE
         * @see IO_BUTTON_FUNCTION
         */
        IO_Button( IO_PUSH_TYPE type, IO_BUTTON_FUNCTION func, IO_Adress adr );

        /**
         * Change button mode
         * Define on witch state button activate it function
         * @see IO_PUSH_TYPE
         */
        void set_type( IO_PUSH_TYPE type );

        /**
         * Get Button Mode
         */
        IO_PUSH_TYPE get_type() const;

        /**
         * Change button function
         * Define what action will be performed at button activation
         * @see IO_BUTTON_FUNCTION
         */
        void set_func( IO_BUTTON_FUNCTION func );

        /**
         * Get Button Function
         */
        IO_BUTTON_FUNCTION get_func() const;

        /**
         * Compute given structure depending on curent button's mode
         */
        void* compute( void* data );

        /**
         * Get Button adress
         */
        IO_Adress get_adr() const;
        

    protected :

        IO_PUSH_TYPE type_;
        IO_BUTTON_FUNCTION func_;
        
        IO_Adress adr_;

        void* data_;
};

namespace HEX{

    #define HEX_UP      0x08    // MCP0 GPIOB 0
    #define HEX_DOWN    0x04    // MCP0 GPIOB 1
    #define HEX_NEXT    0x02    // MCP0 GPIOB 2
    #define HEX_PREV    0x40    // MCP0 GPIOA 3
    #define HEX_ADD     0x80    // MCP0 GPIOA 4
    #define HEX_DEL     0x01    // MCP0 GPIOB 5
    #define HEX_ENTER   0x10    // MCP0 GPIOB 6
    #define HEX_ESC     0x20    // MCP0 GPIOB 7
    #define HEX_OK      0x40    // MCP0 GPIOB 8
    #define HEX_EDIT    0x80    // MCP0 GPIOB 9

    #define MASK_ADRRA_MENU 0xc0// mask for select only Menu buttons
    #define MASK_ADRRB_MENU 0xff// mask for select only Menu buttons
    
    #define HEX_FOOT_NEXT 0x20  // MCP0 GPIOA 10
    #define HEX_FOOT_PREV 0x10  // MCP0 GPIOA 11

    #define MASK_ADRRA_FOOT 0x30 // mask for select only footswitches
}

#endif
