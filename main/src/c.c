/**
 * @file c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h> // 添加此行以包含 printf 函数的声明
#include "lvgl/lvgl.h"
#include "lv_conf.h"

 //添加图片文件
#include "asserts/img/img_bind.c"
#include "asserts/img/img_bind_confirm.c"
#include "asserts/img/img_bind_reject.c"
#include "asserts/img/img_batt_close.c"
#include "asserts/img/img_turn.c"

#include "asserts/img/img_batt_none.c"
#include "asserts/img/img_batt_hightemp.c"
#include "asserts/img/img_batt_lowtemp.c"
#include "asserts/img/img_batt_bar_top.c"

#include "asserts/img/img_type_none.c"
#include "asserts/img/img_type_input.c"
#include "asserts/img/img_type_output.c"
#include "asserts/img/img_type_error.c"
#include "asserts/img/img_type_charge.c"
#include "asserts/img/img_type_discharge.c"
#include "asserts/img/img_type_average.c"
#include "asserts/img/img_type_gather.c"
#include "asserts/img/img_sign_w.c"

#include "asserts/img/img_num_none.c"
#include "asserts/img/img_num_0.c"
#include "asserts/img/img_num_1.c"
#include "asserts/img/img_num_2.c"
#include "asserts/img/img_num_3.c"
#include "asserts/img/img_num_4.c"
#include "asserts/img/img_num_5.c"
#include "asserts/img/img_num_6.c"
#include "asserts/img/img_num_7.c"
#include "asserts/img/img_num_8.c"
#include "asserts/img/img_num_9.c"

/*********************
 *      DEFINES
 *********************/
#define CLEAR lv_color_hex(0x00000000)
#define CONTENT_BG lv_color_hex(0x000000)
#define LINE_BG lv_color_hex(0x333333)

#define CHARGE_SIGN lv_color_hex(0Xb8f09a)
#define INPUT_SIGN lv_color_hex(0x00ff66)

#define ANI_ON lv_color_hex(0x999999)

#define BAR_BG lv_color_hex(0x595959)
#define BAR_GREEN lv_color_hex(0x2aff80)
#define BAR_YELLOW lv_color_hex(0xffcb2a)
#define BAR_RED lv_color_hex(0xff5555)

#define LINE_WIDTH 2
#define BAR_COUNT 3


// 声明图像描述符
LV_IMAGE_DECLARE(bind);
LV_IMAGE_DECLARE(bind_confirm);
LV_IMAGE_DECLARE(bind_reject);
LV_IMAGE_DECLARE(batt_close);
LV_IMAGE_DECLARE(turn);

LV_IMAGE_DECLARE(type_none);
LV_IMAGE_DECLARE(type_input);
LV_IMAGE_DECLARE(type_output);
LV_IMAGE_DECLARE(type_error);
LV_IMAGE_DECLARE(type_charge);
LV_IMAGE_DECLARE(type_average);
LV_IMAGE_DECLARE(type_discharge);
LV_IMAGE_DECLARE(type_gather);

LV_IMAGE_DECLARE(num_none);
LV_IMAGE_DECLARE(num_0);
LV_IMAGE_DECLARE(num_1);
LV_IMAGE_DECLARE(num_2);
LV_IMAGE_DECLARE(num_3);
LV_IMAGE_DECLARE(num_4);
LV_IMAGE_DECLARE(num_5);
LV_IMAGE_DECLARE(num_6);
LV_IMAGE_DECLARE(num_7);
LV_IMAGE_DECLARE(num_8);
LV_IMAGE_DECLARE(num_9);

LV_IMAGE_DECLARE(sign_w);
LV_IMAGE_DECLARE(sign_per);

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    int oval_bg_i;
    int timer_countdown; // 倒计时
} TimerData;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void init_style();
const lv_image_dsc_t * get_num_image(int index);
const lv_image_dsc_t * get_port_status_image(int index);
const lv_image_dsc_t * get_batt_bar_status_image(int index);

//绘制
static void draw_horizontal_oval(int bar_i, lv_obj_t * parent);
static void draw_dot(int bar_i, lv_obj_t * parent);

//电池进度条
static void update_bar_color(int bar_index, int value);
static void update_bar_per(int bar_index, int value);
static void update_bar_status(int bar_index);

static void add_ani(int click_obj_i);
static void bar_value_changed_event_cb(lv_event_t * e);
static void bar_value_reduced_event_cb(lv_event_t * e);

//端口
static void update_port_status(int pow_i);
static void update_port_power(int pow_i);

static void batt_status_changed_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_style_t bg_style;
static lv_style_t batt_style;
static lv_style_t bar_indic;

//端口状态
static int ports_status_data[BAR_COUNT] = {2, 1, 1}; 
static lv_obj_t * ports_status[BAR_COUNT];
const lv_image_dsc_t * port_images[] = {
    &type_none, &type_input, &type_output, &type_error
};

//端口功率
static int powers[BAR_COUNT] = {45, -2, 0};
static lv_obj_t * power_tens_digit[BAR_COUNT];
static lv_obj_t * power_singles_digit[BAR_COUNT];
static lv_obj_t * power_signs[BAR_COUNT];
// 将图像描述符存储在数组中
const lv_image_dsc_t * num_images[] = {
    &num_0, &num_1, &num_2, &num_3, &num_4,
    &num_5, &num_6, &num_7, &num_8, &num_9,
    &num_none
};

//电池工作状态
static int batt_status_click_count = 0;
static lv_obj_t * batt_work_status;
const lv_image_dsc_t * batt_work_images[] = {
    &type_charge, &type_average, &type_discharge, &type_gather
};

//进度条
static int batt_bar_status_data[BAR_COUNT] = {0, 1, 2}; //0有电池, 1未装载, 2高温, 3低温
static lv_obj_t * batt_bar_status[BAR_COUNT];
const lv_image_dsc_t * batt_bar_images[] = {
    NULL, &batt_none, &batt_hightemp, &batt_lowtemp
};
static lv_obj_t * bars_per_none[BAR_COUNT];
static lv_obj_t * bars_per_none1[BAR_COUNT];

static int timer_countdown = 8;
static bool timer_created[BAR_COUNT] = {false, false, false};// 定义标志位数组
static lv_obj_t * ovals_bg[BAR_COUNT];
static lv_obj_t * ovals[BAR_COUNT][3];
static lv_obj_t * bars[BAR_COUNT];
static lv_obj_t * bars_top[BAR_COUNT];
static lv_obj_t * bars_per[BAR_COUNT];
static lv_obj_t * bars_per_sign[BAR_COUNT];


/********************** 
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void c(void)
{
    init_style();

    lv_obj_set_style_bg_color(lv_scr_act(), CONTENT_BG, LV_PART_MAIN);




    lv_obj_t * typeCs_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(typeCs_bg, lv_pct(100), lv_pct(27));
    lv_obj_add_style(typeCs_bg, &bg_style, 0);
    lv_obj_set_style_flex_main_place(typeCs_bg, LV_FLEX_ALIGN_CENTER, 0); //主轴上的内容居中对齐
    lv_obj_set_style_border_side(typeCs_bg, LV_BORDER_SIDE_BOTTOM, 0); // 只绘制底部边框
    lv_obj_set_style_border_width(typeCs_bg, LINE_WIDTH, 0);// 边框宽度
    lv_obj_set_style_border_color(typeCs_bg, LINE_BG, 0);// 底部边框颜色 
    lv_obj_set_style_border_opa(typeCs_bg,  LV_OPA_COVER, 0);// 确保边框不透明
    lv_obj_align(typeCs_bg, LV_ALIGN_TOP_MID, 0, 0);//约束布局

    lv_obj_t * batteries_capacity_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(batteries_capacity_bg, lv_pct(100), lv_pct(73));
    lv_obj_add_style(batteries_capacity_bg, &bg_style, 0);
    lv_obj_set_style_flex_main_place(batteries_capacity_bg, LV_FLEX_ALIGN_SPACE_AROUND, 0);
    //lv_obj_align_to 只会在调用时计算一次对齐位置，不会动态更新
    lv_obj_align_to(batteries_capacity_bg, typeCs_bg, LV_ALIGN_OUT_BOTTOM_MID, 0, 0); 
    
    
    for (int i = 0; i < 4; i++){
        lv_obj_t * obj;
        lv_obj_t * obj_child;

        obj = lv_obj_create(typeCs_bg);
        lv_obj_set_size(obj, LV_PCT(22), LV_PCT(85));
        lv_obj_add_style(obj, &batt_style,0);
        lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0); // 确保背景完全透明
        lv_obj_set_style_bg_color(obj, CLEAR, 0);

        // 设置用户数据,将 int 类型的值转换为 intptr_t 类型，然后再将 intptr_t 类型的值转换为 void* 指针类型。
        lv_obj_set_user_data(obj, (void*)(intptr_t)i);
        lv_obj_add_event_cb(obj, batt_status_changed_event_cb, LV_EVENT_ALL, NULL);
        
        if(i==3){
            //设置电池工作状态
            batt_work_status = lv_image_create(obj);
            lv_image_set_src(batt_work_status, &type_charge);
            lv_obj_align(batt_work_status, LV_ALIGN_CENTER, 0, 0);
            lv_obj_center(batt_work_status);
        }

        if (i<BAR_COUNT){
            //设置端口状态
            ports_status[i] = lv_image_create(obj);
            lv_obj_align(ports_status[i], LV_ALIGN_TOP_MID, 0, 0);
            update_port_status(i);

            //设置端口功率
            power_tens_digit[i] = lv_image_create(obj);
            power_singles_digit[i] = lv_image_create(obj);
            power_signs[i] = lv_image_create(obj);
            lv_image_set_src(power_signs[i], &sign_w);
            update_port_power(i);


            obj = lv_obj_create(batteries_capacity_bg);
            lv_obj_set_size(obj, LV_PCT(27), LV_PCT(93));
            lv_obj_add_style(obj, &batt_style,0);
            lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0); // 确保背景完全透明
            lv_obj_set_style_bg_color(obj, CLEAR, 0);
            lv_obj_set_user_data(obj, (void*)(intptr_t)i);
            lv_obj_add_event_cb(obj, bar_value_reduced_event_cb, LV_EVENT_ALL, NULL); 


            //指示点
            ovals_bg[i] = lv_obj_create(obj); 
            lv_obj_set_size(ovals_bg[i], LV_PCT(17), LV_PCT(13)); 
            lv_obj_add_style(ovals_bg[i], &batt_style,0);
            lv_obj_set_layout(ovals_bg[i], LV_LAYOUT_FLEX);
            lv_obj_set_flex_flow(ovals_bg[i], LV_FLEX_FLOW_COLUMN);
            lv_obj_set_style_flex_main_place(ovals_bg[i], LV_FLEX_ALIGN_SPACE_BETWEEN, 0); 
            lv_obj_set_style_flex_cross_place(ovals_bg[i], LV_FLEX_ALIGN_CENTER, 0);
            lv_obj_set_style_bg_opa(ovals_bg[i], LV_OPA_TRANSP, 0); // 确保背景完全透明
            lv_obj_set_style_bg_color(ovals_bg[i], CLEAR, 0);
            lv_obj_set_align(ovals_bg[i], LV_ALIGN_TOP_MID);

            //电池
            obj_child = lv_obj_create(obj);//电池头
            lv_obj_set_size(obj_child, LV_PCT(78), LV_PCT(66));
            lv_obj_add_style(obj_child, &batt_style,0);
            lv_obj_set_style_bg_opa(obj_child, LV_OPA_TRANSP, 0); // 确保背景完全透明
            lv_obj_set_style_bg_color(obj_child, CLEAR, 0);
            lv_obj_center(obj_child);

            batt_bar_status[i] = lv_image_create(obj_child);
            lv_obj_set_size(batt_bar_status[i], LV_PCT(100), LV_PCT(100));
            lv_image_set_src(batt_bar_status[i], batt_bar_images[1]);
            lv_obj_center(batt_bar_status[i]);

            bars_top[i] = lv_image_create(obj_child);
            lv_image_set_src(bars_top[i],  &batt_bar_top);
            lv_obj_align(bars_top[i], LV_ALIGN_TOP_MID, 0, 1); 

            bars[i] = lv_bar_create(obj_child);//电池进度条
            lv_obj_set_size(bars[i], LV_PCT(100), LV_PCT(95));
            lv_obj_add_style(bars[i], &bar_indic, LV_PART_INDICATOR);
            lv_obj_add_style(bars[i], &batt_style, 0);
            lv_bar_set_value(bars[i], 0, LV_ANIM_OFF);
            lv_obj_align(bars[i], LV_ALIGN_BOTTOM_MID,0,0);
            lv_obj_set_user_data(bars[i], (void*)(intptr_t)i);
            lv_obj_add_event_cb(bars[i], bar_value_changed_event_cb, LV_EVENT_ALL, NULL);  

            //电量百分比
            bars_per_none[i] = lv_image_create(obj);
            lv_image_set_src(bars_per_none[i],  get_num_image(10));
            lv_obj_update_layout(bars_per_none[i]); //强制更新布局
            int num_none = lv_obj_get_width(bars_per_none[i]);// 获取power_ten_digit的宽度
            lv_obj_align(bars_per_none[i], LV_ALIGN_BOTTOM_MID, -(num_none / 2), 0);

            bars_per_none1[i] = lv_image_create(obj);
            lv_image_set_src(bars_per_none1[i],  get_num_image(10)); 
            lv_obj_align_to(bars_per_none1[i], bars_per_none[i], LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);


            bars_per[i] = lv_label_create(obj);
            lv_obj_set_style_text_color(bars_per[i], lv_color_white(), 0);
            lv_obj_set_style_text_font(bars_per[i], (lv_font_t *)&lv_font_montserrat_20, 0);
            // %"LV_PRIu32",指在插入一个 uint32_t 类型的整数值
            lv_label_set_text_fmt(bars_per[i], "%"LV_PRIu32"", 0); 
            lv_obj_align(bars_per[i], LV_ALIGN_BOTTOM_MID, 0, 0);   

            bars_per_sign[i] = lv_label_create(obj);
            lv_obj_set_style_text_color(bars_per_sign[i], lv_color_white(), 0);
            lv_obj_set_style_text_font(bars_per_sign[i], (lv_font_t *)&lv_font_montserrat_10, 0);
            lv_label_set_text_fmt(bars_per_sign[i], "%%");
            lv_obj_align_to(bars_per_sign[i], bars_per[i], LV_ALIGN_OUT_RIGHT_BOTTOM, 2, -2);

            update_bar_status(i);
        }

    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**********************
 *   端口
 **********************/

static void update_port_power(int pow_i){

    int ten = powers[pow_i] / 10;
    int single = powers[pow_i] % 10;
    const lv_image_dsc_t * ten_image = get_num_image(ten); // 获取十位数的图像描述符
    const lv_image_dsc_t * single_image = get_num_image(single); // 获取个位数的图像描述符

    LV_LOG_USER("pow_i-%d, %d, %d", pow_i ,ten, single);

    lv_obj_set_style_opa(power_tens_digit[pow_i], (powers[pow_i] >= 0 && ten == 0 && single >= 0) ? LV_OPA_TRANSP : LV_OPA_COVER, 0);

    if(powers[pow_i] >= 0 && ten == 0 && single >= 0){
        lv_image_set_src(power_singles_digit[pow_i],  (single_image) ? single_image : get_num_image(10));
        lv_obj_align(power_singles_digit[pow_i], LV_ALIGN_BOTTOM_MID, 0, 0);

    }else{
        lv_image_set_src(power_tens_digit[pow_i], (ten_image && powers[pow_i] >= 0) ? ten_image : get_num_image(10));
        lv_image_set_src(power_singles_digit[pow_i],  (single_image && powers[pow_i] >= 0) ? single_image : get_num_image(10));

        lv_obj_update_layout(power_tens_digit[pow_i]); //更改 img 后强制更新布局
    
        int power_ten_digit_width = lv_obj_get_width(power_tens_digit[pow_i]);// 获取power_ten_digit的宽度

        lv_obj_align(power_tens_digit[pow_i], LV_ALIGN_BOTTOM_MID, -(power_ten_digit_width / 2), 0);
        lv_obj_align_to(power_singles_digit[pow_i], power_tens_digit[pow_i], LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
    }

    //符号
    lv_obj_set_style_opa(power_signs[pow_i], (powers[pow_i] >= 0) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    lv_obj_align_to(power_signs[pow_i], power_singles_digit[pow_i], LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0); 

}

// 根据索引获取图像描述符, 返回值类型: const lv_image_dsc_t *
const lv_image_dsc_t * get_num_image(int index) {
    if (index >= 0 && index < sizeof(num_images) / sizeof(num_images[0])) { //不超出数组索引
        return num_images[index];
    }
    return NULL; // 索引无效时返回 NULL
}

static void update_port_status(int pow_i){
    const lv_image_dsc_t * img_status = get_port_status_image(ports_status_data[pow_i]);// 获取图像描述符
    lv_image_set_src(ports_status[pow_i], (img_status) ? img_status : get_port_status_image(0));// 使用图像描述符
}

const lv_image_dsc_t * get_port_status_image(int index) {
    if (index >= 0 && index < sizeof(port_images) / sizeof(port_images[0])) {
        return port_images[index];
    }
    return NULL; // 索引无效时返回 NULL
}

static void batt_status_changed_event_cb(lv_event_t * e){
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * obj = lv_event_get_target(e);
        int click_obj_i = (int)(intptr_t)lv_obj_get_user_data(obj); // 获取用户数据

        batt_status_click_count++;// 增加点击计数

        if(click_obj_i == 3){
            
            const lv_image_dsc_t * img_status = batt_work_images[batt_status_click_count % 4];
            lv_image_set_src(batt_work_status, img_status);

        }else if(click_obj_i == 2){
            
            ports_status_data[2] = batt_status_click_count % 4;
            update_port_status(2);
            
            powers[1] =  batt_status_click_count;
            update_port_power(1);
            
        }
        
    }
}

/**********************
 *   绘制
 **********************/

static void init_style(){
    lv_style_init(&bg_style);
    lv_style_set_bg_opa(&bg_style, LV_OPA_TRANSP);
    lv_style_set_border_opa(&bg_style, LV_OPA_TRANSP); //边框透明
    lv_style_set_border_width(&bg_style,0);
    lv_style_set_radius(&bg_style, 0);//圆角为0
    lv_style_set_pad_all(&bg_style, 0);//内边距为0
    lv_style_set_margin_all(&bg_style, 0);
    lv_style_set_flex_flow(&bg_style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_track_place(&bg_style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(&bg_style, LV_LAYOUT_FLEX); //设置灵活布局
    

    lv_style_init(&batt_style);
    lv_style_set_bg_opa(&batt_style, LV_OPA_COVER); // 确保背景完全不透明
    lv_style_set_bg_color(&batt_style,BAR_BG); 
    lv_style_set_border_opa(&batt_style, LV_OPA_TRANSP); //边框透明
    lv_style_set_border_width(&batt_style,0);
    lv_style_set_radius(&batt_style, 0);//圆角为0
    lv_style_set_pad_all(&batt_style, 0);//内边距为0
    lv_style_set_margin_all(&batt_style, 0);
    


    lv_style_init(&bar_indic);
    lv_style_set_bg_opa(&bar_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&bar_indic, BAR_RED);
    lv_style_set_radius(&bar_indic, 0);

}

static void draw_horizontal_oval(int bar_i, lv_obj_t * parent){
    for(int i=0; i<3; i++){
        ovals[bar_i][i] = lv_obj_create(parent);
        lv_obj_set_size(ovals[bar_i][i], LV_PCT(100), lv_pct(24));
        lv_obj_add_style(ovals[bar_i][i], &batt_style, 0);
        lv_obj_set_style_radius(ovals[bar_i][i], 12, 0);
        lv_obj_set_style_bg_color(ovals[bar_i][i], LINE_BG, 0);
    }
}

static void draw_dot(int bar_i, lv_obj_t * parent){
    for(int i=0; i<3; i++){
        ovals[bar_i][i] = lv_obj_create(parent);
        lv_obj_set_size(ovals[bar_i][i], LV_PCT(58), LV_PCT(24));
        lv_obj_add_style(ovals[bar_i][i], &batt_style, 0);
        lv_obj_set_style_radius(ovals[bar_i][i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(ovals[bar_i][i], LINE_BG, 0);
    }
}


/**********************
 *   进度条
 **********************/

static void update_bar_status(int bar_index){
    int index = batt_bar_status_data[bar_index]; 
    const lv_image_dsc_t * img_status = get_batt_bar_status_image(index);// 获取图像描述符
    lv_image_set_src(batt_bar_status[bar_index], (img_status) ? img_status : batt_bar_images[1]);

    lv_obj_set_style_opa(batt_bar_status[bar_index], (index != 0) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    lv_obj_set_style_opa(bars_top[bar_index], (index == 0) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    lv_obj_set_style_opa(bars[bar_index], (index == 0) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);

    //状态为1,未装载, 电量不显示
    lv_obj_set_style_opa(bars_per[bar_index], (index != 1) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    lv_obj_set_style_opa(bars_per_sign[bar_index], (index != 1) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    lv_obj_set_style_opa(bars_per_none[bar_index], (index == 1) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    lv_obj_set_style_opa(bars_per_none1[bar_index], (index == 1) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);

}

const lv_image_dsc_t * get_batt_bar_status_image(int index) {
    if (index >= 0 && index < sizeof(batt_bar_images) / sizeof(batt_bar_images[0])) {
        return batt_bar_images[index];
    }
    return NULL; // 索引无效时返回 NULL
}

static void update_bar_per(int bar_index, int value){
    if(value >= 0 && value <= 100){
        lv_label_set_text_fmt(bars_per[bar_index], "%"LV_PRIu32"", value); 
        // 重新对齐 label
        lv_obj_align_to(bars_per_sign[bar_index], bars_per[bar_index], LV_ALIGN_OUT_RIGHT_BOTTOM, 2, -2);
    }
}

static void update_bar_color(int bar_index, int value){
    if(value >= 0 && value <= 100){
        if (value > 10 && value < 80) {
            printf("Value is between 10 and 80\n");
            lv_obj_set_style_bg_color(bars[bar_index], BAR_YELLOW, LV_PART_INDICATOR);
        } else if (value >= 80 && value <= 100) {
            printf("Value is between 80 and 100\n");
            lv_obj_set_style_bg_color(bars[bar_index], BAR_GREEN, LV_PART_INDICATOR);

            // if(value == 100){
            //     lv_obj_set_style_img_recolor(bars_top[bar_index], BAR_GREEN, 0);
            // }
            // lv_obj_set_style_bg_color(bars_top[bar_index], (value == 100) ? BAR_GREEN : BAR_BG, 0);
        } else {
            printf("Value is between 0 and 10\n");
            lv_obj_set_style_bg_color(bars[bar_index], BAR_RED, LV_PART_INDICATOR);
            // lv_obj_set_style_bg_color(bars_top[bar_index], BAR_BG, 0);
        }
        lv_obj_set_style_image_opa(bars_top[bar_index], LV_OPA_COVER, 0);
        lv_obj_set_style_img_recolor(bars_top[bar_index], (value == 100) ? BAR_GREEN  : BAR_BG, 0);
    }
}

// 进度条定时器回调函数
static void ani_timer_event_cb(lv_timer_t * timer) {

    TimerData * timer_data = (TimerData *)lv_timer_get_user_data(timer); // 获取定时器的用户数据
    if (!timer_data) return; // 确保数据有效

    int oval_bg_i = timer_data->oval_bg_i; // 获取用户数据
    timer_data->timer_countdown--; // 减少倒计时 
    
    if(timer_data->timer_countdown >= 0){

        // char buf[8]; // 足够存储两位数加冒号如 "59" 或 "0:59"
        // sprintf(buf, "%d", timer_data->timer_countdown);
        // lv_label_set_text(time_label, buf); // 更新标签文本

        lv_obj_set_style_bg_color(ovals[oval_bg_i][0],LINE_BG, 0);
        lv_obj_set_style_bg_color(ovals[oval_bg_i][1],LINE_BG, 0);
        lv_obj_set_style_bg_color(ovals[oval_bg_i][2],LINE_BG, 0);

        int i = (11 - timer_data->timer_countdown) % 4;
        // LV_LOG_USER("ovals- %d : %d , %d", oval_bg_i, timer_data->timer_countdown, i);
        if (i >= 0 && i <= 2){
            lv_obj_set_style_bg_color(ovals[oval_bg_i][i],ANI_ON, 0);
        }

    } else {
        // lv_label_set_text(time_label, "Done!"); // 当倒计时结束时显示完成信息
        lv_obj_clean(ovals_bg[oval_bg_i]);//清除指示点动画
        timer_data->timer_countdown = timer_countdown;

        
        lv_timer_del(timer); // 删除定时器
        lv_free(timer_data); // 释放分配的内存
        timer_created[oval_bg_i] = false;
    }
}

static void add_ani(int click_obj_i){
    // 检查是否已经创建了定时器
    if (timer_created[click_obj_i]) {
        LV_LOG_USER("Timer already created for index %d", click_obj_i);
        return;
    }

    //增加指示点 ui
    if(click_obj_i==2){
        lv_obj_set_style_flex_track_place(ovals_bg[2], LV_FLEX_ALIGN_CENTER, 0);   
        draw_dot(click_obj_i, ovals_bg[click_obj_i]);
    }else{
        draw_horizontal_oval(click_obj_i, ovals_bg[click_obj_i]);  
    }

    // 动态分配 TimerData 结构体内存
    TimerData * oval_data = (TimerData *)lv_malloc(sizeof(TimerData)); 
    if (!oval_data) {
        LV_LOG_USER("Failed to allocate memory for TimerData");
        return;
    }
    oval_data->oval_bg_i = click_obj_i;
    oval_data->timer_countdown = timer_countdown; // 初始化倒计时
    // 创建指示点动画定时器并传递用户数据 oval_data,每隔1000ms触发一次
    lv_timer_t * timer = lv_timer_create(ani_timer_event_cb, 1000, oval_data);
    timer_created[click_obj_i] = true; // 标记定时器已创建

}

// 进度条值变化事件
static void bar_value_changed_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * obj = lv_event_get_target(e);
        int click_obj_i = (int)(intptr_t)lv_obj_get_user_data(obj); // 获取用户数据
        // LV_LOG_USER("bar_value_changed_event_cb, click_obj_index: %d ", click_obj_i);

        if (click_obj_i >= 0 && click_obj_i < BAR_COUNT) {

            if(batt_bar_status_data[click_obj_i] == 0){
                //增进度条
                int value = lv_bar_get_value(bars[click_obj_i]) + 10;
                lv_bar_set_value(bars[click_obj_i], value, LV_ANIM_ON);
                // update_bar_color(click_obj_i, value);
                update_bar_color(click_obj_i, 100);
                update_bar_per(click_obj_i, 100);

                add_ani(click_obj_i);
            }
 
        }
    }
}

static void bar_value_reduced_event_cb(lv_event_t * e){
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * obj = lv_event_get_target(e);
        int click_obj_i = (int)(intptr_t)lv_obj_get_user_data(obj); // 获取用户数据
        if (click_obj_i >= 0 && click_obj_i < BAR_COUNT) {
            //减进度条
            int value = lv_bar_get_value(bars[click_obj_i]) - 10;
            lv_bar_set_value(bars[click_obj_i], value, LV_ANIM_ON);
            update_bar_color(click_obj_i, value);
            update_bar_per(click_obj_i, value);

            //改变电池进度条状态
            batt_bar_status_data[click_obj_i] = value / 10;
            update_bar_status(click_obj_i);
            
        }
    }
}



