#include "tof_gestures.h"
#include "tof_gestures_DIRSWIPE_1.h"

int tof_gestures_initDIRSWIPE_1(int32_t rangeThreshold_mm, long minSwipeDuration, long maxSwipeDuration, Gesture_DIRSWIPE_1_Data_t *data){
    int status=0;
    // Init left and right motion
    status |= tof_initMotion(rangeThreshold_mm, &(data->motionDetectorLeft));
    status |= tof_initMotion(rangeThreshold_mm, &(data->motionDetectorRight));
    data->minSwipeDuration = minSwipeDuration;
    data->maxSwipeDuration = maxSwipeDuration;
    data->state = GESTURES_DIRSWIPE_START;
    data->timestamp = 0;
    data->gesture_start_from_right = -1;
    return status;
}

DirSwipe_Info tof_gestures_detectDIRSWIPE_1(int32_t left_range_mm, int32_t right_range_mm, Gesture_DIRSWIPE_1_Data_t *data){
    int return_code = GESTURES_NULL;
    int r_motion_code, l_motion_code;
    int start_from_right; 
    unsigned long timestamp = GET_TIME_STAMP();
    long duration=0;
    DirSwipe_Info returnInfo;
    
    // Get left and right motion
    l_motion_code = tof_getMotion(left_range_mm, &(data->motionDetectorLeft));
    r_motion_code = tof_getMotion(right_range_mm, &(data->motionDetectorRight));
    
    //TOF_GESTURES_DEBUG(DIRSWIPE_1,"lmotion=%d, rmotion=%d", l_motion_code, r_motion_code);
    
    switch(data->state){
        case GESTURES_DIRSWIPE_START:
            if (l_motion_code == GESTURES_MOTION_DROP_DOWN){
                start_from_right = 0;
                data->timestamp = timestamp;
                data->state = GESTURES_L_PENDING;
            }else if (r_motion_code == GESTURES_MOTION_DROP_DOWN){
                start_from_right = 1;
                data->timestamp = timestamp;
                data->state = GESTURES_R_PENDING;
            }
            break;
        case GESTURES_R_PENDING: 
            if(r_motion_code == GESTURES_MOTION_RAISE_UP){
                data->state = GESTURES_R_END; 
            }
            else if(l_motion_code == GESTURES_MOTION_DROP_DOWN){
                data->state = GESTURES_TAP_PENDING;
            }
            // pending too long 
            else if (timestamp - data->timestamp > 1000){
                data->state = GESTURES_WAIT;
            }
            break;

         case GESTURES_L_PENDING: 
            //swipe detected 
            if(l_motion_code == GESTURES_MOTION_RAISE_UP){
               data->state = GESTURES_L_END; 
            }
            else if(r_motion_code == GESTURES_MOTION_DROP_DOWN){
                data->state = GESTURES_TAP_PENDING;
            }
            // pending too long 
            else if (timestamp - data->timestamp > 1000){
                data->state = GESTURES_WAIT;
            }
            break;
            
        case GESTURES_R_END:
            duration = timestamp - data->timestamp;
            if(duration > data->minSwipeDuration){
                return_code = GESTURES_SWIPE_RIGHT_LEFT;
                data->state = GESTURES_WAIT;
    
            }else{
                return_code = GESTURES_DISCARDED_TOO_FAST;
            }
            break;

        case GESTURES_L_END:
            duration = timestamp - data->timestamp;
            if(duration > data->minSwipeDuration){
                return_code = GESTURES_SWIPE_LEFT_RIGHT;
                data->state = GESTURES_WAIT;
    
            }else{
                return_code = GESTURES_DISCARDED_TOO_FAST;
            }
            break;

        case GESTURES_WAIT:
            if ((l_motion_code == GESTURES_MOTION_UP_STATE) && (r_motion_code == GESTURES_MOTION_UP_STATE)){
                data->state = GESTURES_DIRSWIPE_START;
            }
            break;
        case GESTURES_TAP_PENDING:
            if (r_motion_code == GESTURES_MOTION_RAISE_UP){
                data->state = GESTURES_R_END;
            }
            else if (l_motion_code == GESTURES_MOTION_RAISE_UP){
                data->state = GESTURES_L_END;
            }
            else if (timestamp - data->timestamp > 700){
                return_code = GESTURES_SINGLE_TAP;
                data->state = GESTURES_WAIT;
            }
            
            break;
    }
    returnInfo.returnCode = return_code;
    returnInfo.duration = duration;
    return returnInfo;
}