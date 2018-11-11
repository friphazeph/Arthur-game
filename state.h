
#ifndef __GAME__STATE_H
    #define __GAME__STATE_H
    
    #define DEFAULT_STATE 0
    #define STICKY_STATE  1
    #define FLYING_STATE  2
    
    typedef struct {
        var type;
        
        var  vacc; // vertical acceleration
        var  impeed;
        bool can_double_jump;
        
    } state_t;
    
    void default_state(state_t* this) {
        this->type = DEFAULT_STATE;
        
        this->vacc            = 1;
        this->impeed          = 2;
        this->can_double_jump = false;
        
    }
    
    void sticky_state(state_t* this) {
        this->type = STICKY_STATE;
        
        this->vacc            = 3;
        this->impeed          = 4;
        this->can_double_jump = false;
        
    }
    
    void flying_state(state_t* this) {
        this->type = FLYING_STATE;
        
        this->vacc            = -2;
        this->impeed          = 1;
        this->can_double_jump = true;
        
    }
    
#endif
