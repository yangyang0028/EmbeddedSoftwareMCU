/**
  ****************************(C) COPYRIGHT 2021 HRBUST_AIR****************************
  * @file    Judge_Data.c
	* @brief   DJI����ϵͳ���ݷ��ͺ���           
  * @author  JackyJuu , HRBUST_AIR_TEAM , website:www.airclub.tech
	* @Note 	 ������Ʋ���ϵͳ���ͽ��պ���������ʾ
  * @version V1.0.0
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     12-2-2020      JackyJuu            Done
  *  V1.2.0     4-2-2021      JackyJuu            Done
  ****************************(C) COPYRIGHT 2021 HRBUST_AIR****************************
	* @describe DJI����ϵͳ��������
*/
#include "Judge_Data.h"
#include "Judge_CRC.h"

#define    FALSE    0
#define    TRUE     1

DJI_Judge_t DJI_Judge; 

uint8_t Judge_Send_Dataa[200];

uint8_t Judge_Student_Communicate_Dataa[200];
int Judge_Send_Data_Long,Judge_Send_Data_Long_1,i;



/*****************************
****����ϵͳ�ͻ��˻���ֱ��****
���룺
		Graphic_width		��������
		Graphic_start_x	���x����
		Graphic_start_y	���y����
		Graphic_end_x		�յ�x����
		Graphic_end_y		�յ�Y����
******************************/	
void Judge_Straight_Line_Write(graphic_data_struct_t* Line_data,int Graphic_start_x,int Graphic_start_y,int Graphic_end_x,int Graphic_end_y)
{
	Line_data->start_x = (uint32_t)Graphic_start_x;
	Line_data->start_y = (uint32_t)Graphic_start_y;
	Line_data->end_x = (uint32_t)Graphic_end_x;
	Line_data->end_y = (uint32_t)Graphic_end_y;	
}

/*****************************
****����ϵͳ�ͻ��˻��ƾ���****
���룺
		Graphic_width		��������
		Graphic_start_x	���x����
		Graphic_start_y	���y����
		Graphic_end_x		�ԽǶ���x����
		Graphic_end_y		�ԽǶ���Y����
******************************/	
void Judge_Rectangle_Write(graphic_data_struct_t* Rectangle_data,int Graphic_start_x,int Graphic_start_y,int Graphic_end_x,int Graphic_end_y)
{
	Rectangle_data->start_x = (uint32_t)Graphic_start_x;
	Rectangle_data->start_y = (uint32_t)Graphic_start_y;
	Rectangle_data->end_x = (uint32_t)Graphic_end_x;
	Rectangle_data->end_y = (uint32_t)Graphic_end_y;	
}

/*****************************
****����ϵͳ�ͻ��˻�����Բ****
���룺
		Graphic_width		��������
		Graphic_start_x	Բ��x����
		Graphic_start_y	Բ��y����
		Graphic_radius	�뾶
******************************/	
void Judge_All_Circle_Write(graphic_data_struct_t* All_Circle_data,int Graphic_start_x,int Graphic_start_y,int Graphic_radius)
{
	All_Circle_data->start_x = (uint32_t)Graphic_start_x;
	All_Circle_data->start_y = (uint32_t)Graphic_start_y;
	All_Circle_data->radius = (uint32_t)Graphic_radius;
}

/******************************
****����ϵͳ�ͻ��˻�����Բ****
���룺
		Graphic_width		��������
		Graphic_start_x	Բ��x����
		Graphic_start_y	Բ��y����
		Graphic_end_x		x���᳤��
		Graphic_end_y		y���᳤��
******************************/	
void Judge_Oval_Circle_Write(graphic_data_struct_t* Oval_Circl_data,int Graphic_start_x,int Graphic_start_y,int Graphic_end_x,int Graphic_end_y)
{
	Oval_Circl_data->start_x = (uint32_t)Graphic_start_x;
	Oval_Circl_data->start_y = (uint32_t)Graphic_start_y;
	Oval_Circl_data->end_x = (uint32_t)Graphic_end_x;
	Oval_Circl_data->end_y = (uint32_t)Graphic_end_y;	
}

/******************************
****����ϵͳ�ͻ��˻���Բ��****
���룺
		Graphic_start_angle	��ʼ�Ƕ�
		Graphic_end_angle		��ֹ�Ƕ�
		Graphic_width		��������
		Graphic_start_x	Բ��x����
		Graphic_start_y	Բ��y����
		Graphic_end_x		x���᳤��
		Graphic_end_y		y���᳤��
******************************/	
void Judge_Arc_Circle_Write(graphic_data_struct_t* Arc_Circle_data,int Graphic_start_angle,int Graphic_end_angle,int Graphic_start_x,int Graphic_start_y,int Graphic_end_x,int Graphic_end_y)
{
	Arc_Circle_data->start_angle = (uint32_t)Graphic_start_angle;
	Arc_Circle_data->end_angle = (uint32_t)Graphic_end_angle;
	Arc_Circle_data->start_x = (uint32_t)Graphic_start_x;
	Arc_Circle_data->start_y = (uint32_t)Graphic_start_y;
	Arc_Circle_data->end_x = (uint32_t)Graphic_end_x;
	Arc_Circle_data->end_y = (uint32_t)Graphic_end_y;	
}

/******************************
****����ϵͳ�ͻ��˻��Ƹ�����****
���룺
		Word_width	�����С
		Float_Small_Num	С��λ��Ч����
		Graphic_width		��������
		Graphic_start_x	���x����
		Graphic_start_y	���y����
		Floating_Num		32λ��������float
******************************/	
void Judge_Floating_Write(Word_data_struct_t* Float_data_Set,int Word_width,int Float_Small_Num,int Graphic_start_x,int Graphic_start_y,float Floatint_Data)
{
	
	Float_data_Set->start_angle = (uint32_t)Word_width;
	Float_data_Set->end_angle = (uint32_t)Float_Small_Num;	
	Float_data_Set->start_x = (uint32_t)Graphic_start_x;
	Float_data_Set->start_y = (uint32_t)Graphic_start_y;
	memcpy(Float_data_Set->Word_Data,(uint8_t *)&Floatint_Data,4); 	
	
//	Float_data_Set->Word_Data[0] = (uint8_t)((uint32_t)Floatint_Data);
//	Float_data_Set->Word_Data[1] = (uint8_t)(((uint32_t)Floatint_Data << 8));
//	Float_data_Set->Word_Data[2] = (uint8_t)(((uint32_t)Floatint_Data << 16));
//	Float_data_Set->Word_Data[3] = (uint8_t)(((uint32_t)Floatint_Data << 24));
	
//	for(int i = 0;i < 4;i++)
//	memcpy(Float_data_Set->Word_Data + i,(uint8_t *)&Floatint_Data + i,2);
//	memcpy(Float_data_Set->Word_Data + 0,(uint8_t *)&Floatint_Data + 3,2);
//	memcpy(Float_data_Set->Word_Data + 1,(uint8_t *)&Floatint_Data + 2,2);
//	memcpy(Float_data_Set->Word_Data + 2,(uint8_t *)&Floatint_Data + 1,2);
//	memcpy(Float_data_Set->Word_Data + 3,(uint8_t *)&Floatint_Data + 0,2); 	
//	memcpy(Float_data_Set->Word_Data + 1,(uint8_t *)&Floatint_Data,sizeof(uint8_t)); 	
//	memcpy(Float_data_Set->Word_Data + 2,(uint8_t *)&(Floatint_Data >> 8),sizeof(uint8_t)); 	
//	memcpy(Float_data_Set->Word_Data + 3,(uint8_t *)&Floatint_Data,sizeof(uint8_t)); 	
}

/******************************
****����ϵͳ�ͻ��˻���������****
���룺
		Word_width	�����С
		Float_Small_Num	С��λ��Ч����
		Graphic_width		��������
		Graphic_start_x	���x����
		Graphic_start_y	���y����
		Floating_Num		32λ��������int32_t
******************************/	
void Judge_Integer_Write(Word_data_struct_t* Integer_data_Set,int Word_width,int Graphic_start_x,int Graphic_start_y,int Int_Num)
{
	Integer_data_Set->start_angle = (uint32_t)Word_width;
	Integer_data_Set->start_x = (uint32_t)Graphic_start_x;
	Integer_data_Set->start_y = (uint32_t)Graphic_start_y;
	memcpy(Integer_data_Set->Word_Data,(uint8_t *)&Int_Num,4); 
}

/******************************
****����ϵͳ�ͻ��˻����ַ�****
���룺
		Word_width	�����С
		Word_Lenth	�ַ�����
		Graphic_width		��������
		Graphic_start_x	���x����
		Graphic_start_y	���y����
******************************/	
void Judge_Character_Write(graphic_data_struct_t* Character_data,int Word_width,int Word_Lenth,int Graphic_start_x,int Graphic_start_y)
{
	Character_data->start_angle = (uint32_t)Word_width;
	Character_data->end_angle = (uint32_t)Word_Lenth;		
	Character_data->start_x = (uint32_t)Graphic_start_x;
	Character_data->start_y = (uint32_t)Graphic_start_y;
}


//void Judge_Graphic_Data_Calculate(uint32_t dataaa,graphic_data_struct_t* graphic_data_size_test)
//{
//	dataaa = (graphic_data_size_test->graphic_tpye <<29) || 
//}
uint32_t dataaa[3];
/***************************************
****����ϵͳ�ͻ����Զ���ͼ����������****
���룺(���в���)
		Graphic_Name	ͼ����
		Judge_Graphic_Type	ͼ�β���
		Judge_Graphic_Control	ͼ������
		Grap_Board_Num	ͼ����
		Grap_Colour	��ɫ
		Grap_start_angle	��ʼ�Ƕ�
		Grap_end_angle		��ֹ�Ƕ�
		Grap_width		��������
		Grap_start_x	���x����
		Grap_start_y	���y����
		Grap_size			�����С���߰뾶
		Grap_end_x		�յ�x����
		Grap_end_y		�յ�y����
*****************************************/	
void Judge_Graphic_Data_Set(graphic_data_struct_t* graphic_data,uint8_t* Graphic_Name,Judge_Graphic_Type_t Judge_Graphic_Type,Judge_Graphic_Control_t Judge_Graphic_Control,\
int Grap_Board_Num,int Grap_Colour,int Grap_start_angle,int Grap_end_angle,int Grap_width,int Grap_start_x,int Grap_start_y,int Grap_size,int Grap_end_x,int Grap_end_y)
{

	graphic_data->graphic_name[0] = *Graphic_Name;
	graphic_data->graphic_name[1] = *Graphic_Name+1;
	graphic_data->graphic_name[2] = *Graphic_Name+2;
	
//	dataaa = (Judge_Graphic_Control << 29)|(Judge_Graphic_Type << 29)|(Grap_Board_Num << 28)|(Grap_Colour << 28)|(Grap_start_angle << 21)|(Grap_end_angle << 21);
	
	graphic_data->operate_tpye = Judge_Graphic_Control;
	graphic_data->graphic_tpye = Judge_Graphic_Type;
	graphic_data->layer = Grap_Board_Num;
	graphic_data->color = Grap_Colour;
	graphic_data->width = Grap_width;
//	memcpy(dataaa,graphic_data+6,12);
	switch(Judge_Graphic_Type)
	{
		case Straight_Line:
		{
			Judge_Straight_Line_Write(graphic_data,Grap_start_x,Grap_start_y,Grap_end_x,Grap_end_y);
			break;
		}
		case Rectangle:
		{
			Judge_Rectangle_Write(graphic_data,Grap_start_x,Grap_start_y,Grap_end_x,Grap_end_y);
			break;
		}
		case All_Circle:
		{
			Judge_All_Circle_Write(graphic_data,Grap_start_x,Grap_start_y,Grap_size);
			break;
		}
		case Oval_Circle:
		{
			Judge_Oval_Circle_Write(graphic_data,Grap_start_x,Grap_start_y,Grap_end_x,Grap_end_y);
			break;
		}
		case Arc_Circle:
		{
			Judge_Arc_Circle_Write(graphic_data,Grap_start_angle,Grap_end_angle,Grap_start_x,Grap_start_y,Grap_end_x,Grap_end_y);
			break;
		}
		case Floating:
//			Judge_Floating_Write(graphic_data,Grap_start_angle,Grap_end_angle,Grap_start_x,Grap_start_y);
		break;
		case Integer:
	//		Judge_Integer_Write(graphic_data,Grap_start_angle,Grap_start_x,Grap_start_y);
		break;
		case Character:
//			Judge_Character_Write(graphic_data,Grap_start_angle,Grap_end_angle,Grap_start_x,Grap_start_y);
		break;
		
	
	}

}

/***************************************
****����ϵͳ�ͻ����Զ�������������****
���룺(���в���)
		Graphic_Name	ͼ����
		Judge_Graphic_Type	ͼ�β���
		Judge_Graphic_Control	ͼ������
		Grap_Board_Num	ͼ����
		Grap_Colour	��ɫ
		Grap_Word_Size	�����С
		Grap_Word_Lenth		С��λ��Ч����/�ַ�����
		Grap_width		��������
		Grap_start_x	���x����
		Grap_start_y	���y����
		Flost_32_Data		������32λ����		
		Int_32_Data			����32λ����
		Send_Character	�ַ�����ַ
*****************************************/	
void Judge_Word_Data_Set(Word_data_struct_t* Word_data_Set,uint8_t* Graphic_Name,Judge_Graphic_Type_t Judge_Graphic_Type,Judge_Graphic_Control_t Judge_Graphic_Control,\
int Grap_Board_Num,int Grap_Colour,int Grap_Word_Size,int Grap_Word_Lenth,int Grap_width,int Grap_start_x,int Grap_start_y,float Flost_32_Data,int32_t Int_32_Data)
{
	
	Word_data_Set->graphic_name[0] = *Graphic_Name;
	Word_data_Set->graphic_name[1] = *Graphic_Name+1;
	Word_data_Set->graphic_name[2] = *Graphic_Name+2;
	
	Word_data_Set->operate_tpye = Judge_Graphic_Control;
	Word_data_Set->graphic_tpye = Judge_Graphic_Type;
	Word_data_Set->layer = Grap_Board_Num;
	Word_data_Set->color = Grap_Colour;
	Word_data_Set->width = Grap_width;
	switch(Judge_Graphic_Type)
	{
		case Floating:
		{
			Judge_Floating_Write(Word_data_Set,Grap_Word_Size,Grap_Word_Lenth,Grap_start_x,Grap_start_y,Flost_32_Data);
			break;
		}
		case Integer:
		{
			Judge_Integer_Write(Word_data_Set,Grap_Word_Size,Grap_start_x,Grap_start_y,Int_32_Data);
			break;
		}
		case Character:
		{
//		Judge_Character_Write(graphic_data,Grap_Word_Size,Grap_Word_Lenth,Grap_start_x,Grap_start_y);
			break;
		}
		case Straight_Line:break;
		case Rectangle:break;
		case All_Circle:break;
		case Oval_Circle:break;
		case Arc_Circle:break;	
	
	}
	
}
extern uint8_t	Judge_Test_Graphic_Data[];
/***************************************
****����ϵͳ�ͻ����Զ����ַ���������****
���룺(���в���)
		Graphic_Name	ͼ����
		Judge_Graphic_Type	ͼ�β���
		Judge_Graphic_Control	ͼ������
		Grap_Board_Num	ͼ����
		Grap_Colour	��ɫ
		Grap_Word_Size	�����С
		Grap_Word_Lenth		�ַ�����
		Grap_width		��������
		Grap_start_x	���x����
		Grap_start_y	���y����
		Send_Character	�ַ�����ַ
*****************************************/	
void Judge_Character_Data_Set(Judge_client_custom_character_t* Judge_client_custom_character_Set,uint8_t* Graphic_Name,Judge_Graphic_Type_t Judge_Graphic_Type,Judge_Graphic_Control_t Judge_Graphic_Control,\
int Grap_Board_Num,int Grap_Colour,int Grap_Word_Size,int Grap_Word_Lenth,int Grap_width,int Grap_start_x,int Grap_start_y,uint8_t* Send_Character)
{

	Judge_client_custom_character_Set->grapic_data_struct.graphic_name[0] = *Graphic_Name;
	Judge_client_custom_character_Set->grapic_data_struct.graphic_name[1] = *Graphic_Name+1;
	Judge_client_custom_character_Set->grapic_data_struct.graphic_name[2] = *Graphic_Name+2;
		
	Judge_client_custom_character_Set->grapic_data_struct.operate_tpye = Judge_Graphic_Control;
	Judge_client_custom_character_Set->grapic_data_struct.graphic_tpye = Judge_Graphic_Type;
	Judge_client_custom_character_Set->grapic_data_struct.layer = Grap_Board_Num;
	Judge_client_custom_character_Set->grapic_data_struct.color = Grap_Colour;
	Judge_client_custom_character_Set->grapic_data_struct.width = Grap_width;
	switch(Judge_Graphic_Type)
	{
		case Floating:break;
		case Integer:break;
		case Character:
			Judge_Character_Write(&Judge_client_custom_character_Set->grapic_data_struct,Grap_Word_Size,Grap_Word_Lenth,Grap_start_x,Grap_start_y);
			memcpy(Judge_client_custom_character_Set->data,Send_Character,30);			
			break;
		case Straight_Line:break;
		case Rectangle:break;
		case All_Circle:break;
		case Oval_Circle:break;
		case Arc_Circle:break;		
	}
}

/***************************************
****����ϵͳ�ͻ���ͼ��ɾ������****
���룺(���в���)
		graphic_Set	ͼ�β���
		graphic_Num	ͼ����
*****************************************/
void Judge_Delete_Data_Send(union Judge_Graphic_Delet_t *	Judge_client_custom_graphic_delete_Set,uint8_t graphic_Set,uint8_t graphic_Num)
{
	Judge_client_custom_graphic_delete_Set->Judge_client_custom_graphic_delete.layer = graphic_Num;
	Judge_client_custom_graphic_delete_Set->Judge_client_custom_graphic_delete.operate_tpye = graphic_Set;
}

void Judge_Communication_To_Client_Custom(DJI_Judge_t*DJI_Judge_Clint_Communication)
{
	
}


/***************************************
****����ϵͳ�ͻ����Զ����ַ����ݷ���****
���룺(���в���)
Stu_data_cmd_id	����ID
		Stu_Get_id	������ID
		p_data	����
		Data_long	���ݳ���
*****************************************/	
void Judge_Data_Send(DJI_Judge_t* DJI_Judge_Send_Data,uint16_t Stu_data_cmd_id,uint16_t Stu_Get_id, uint8_t* p_data , int Data_long)
{
	static uint8_t Judge_Send_Seq = 0;

//	Judge_Send_Data_Long_1 = sizeof(&p_data);
	
	DJI_Judge_Send_Data->Judge_Send_Data.frame_header.sof = DJI_Judge_Header_SOF;
	DJI_Judge_Send_Data->Judge_Send_Data.frame_header.data_length = sizeof(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data) + Data_long;
	DJI_Judge_Send_Data->Judge_Send_Data.frame_header.seq = Judge_Send_Seq;
	

	
	memcpy(Judge_Send_Dataa, &DJI_Judge_Send_Data->Judge_Send_Data.frame_header, sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header));
	append_crc8_check_sum(Judge_Send_Dataa,sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header));
	
	DJI_Judge_Send_Data->Judge_Send_Data.cmdid = DJI_Judge_Student_CmdID;
	DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data.data_cmd_id = Stu_data_cmd_id;
	DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data.sender_ID = \
	(uint16_t)DJI_Judge_Send_Data->DJI_Judge_Mes.Judge_game_robot_status.robot_id;
	DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data.receiver_ID = Stu_Get_id;
	
	
	
	memcpy(Judge_Send_Dataa+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header),(uint8_t *)&DJI_Judge_Send_Data->Judge_Send_Data.cmdid,sizeof(DJI_Judge_Send_Data->Judge_Send_Data.cmdid));
	
	memcpy(Judge_Send_Dataa+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header)+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.cmdid),(uint8_t*)&DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data,sizeof(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data)); 	

	memcpy(Judge_Send_Dataa+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header)+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.cmdid) + sizeof(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data),p_data,Data_long); 		


//	memcpy(Judge_Send_Dataa + sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header) + sizeof(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data), p_data, sizeof(&p_data));
	append_crc16_check_sum(Judge_Send_Dataa,sizeof(DJI_Judge_Send_Data->Judge_Send_Data) + Data_long);
	Judge_Send_Data_Long = sizeof(DJI_Judge_Send_Data->Judge_Send_Data) + Data_long;

		for(i = 0;i < Judge_Send_Data_Long;i++)
		{
			USART6_SendData(Judge_Send_Dataa[i]);
//			while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
//			USART_SendData(USART6,(uint8_t)Judge_Send_Dataa[i]);			
		}	
	
	Judge_Send_Seq++;
	if(Judge_Send_Seq == 0xFF)
	{
		Judge_Send_Seq = 0;
	}
}

uint16_t Get_Robot_Client_Id(uint16_t Robot_Id)
{
	switch(Robot_Id)
	{
		case 1:
			return 0x0101;
		break;
		case 2:
			return 0x0102;
		break;
		case 3:
			return 0x0103;
		break;
		case 4:
			return 0x0104;
		break;
		case 5:
			return 0x0105;
		break;
		case 6:
			return 0x0106;
		break;
		case 101:
			return 0x0165;
		break;
		case 102:
			return 0x0167;
		break;
		case 103:
			return 0x0167;
		break;
		case 104:
			return 0x0168;
		break;
		case 105:
			return 0x0169;
		break;
		case 106:
			return 0x016A;
		break;	
	}
	return 0;
}

/***************************************
****����ϵͳ�������͵��ͻ���****
���룺(���в���)
		Stu_data_cmd_id	����ID
		Stu_Get_id	������ID
		p_data	����
		Data_long	���ݳ���
*****************************************/
void Judge_Data_Send_To_Client(DJI_Judge_t* DJI_Judge_Send_Data,uint16_t Stu_data_cmd_id, uint8_t* p_data , int Data_long)
{
	static uint8_t Judge_Send_Seq = 0;

//	Judge_Send_Data_Long_1 = sizeof(&p_data);
	
	DJI_Judge_Send_Data->Judge_Send_Data.frame_header.sof = DJI_Judge_Header_SOF;
	DJI_Judge_Send_Data->Judge_Send_Data.frame_header.data_length = sizeof(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data) + Data_long;
	DJI_Judge_Send_Data->Judge_Send_Data.frame_header.seq = Judge_Send_Seq;
	

	
	memcpy(Judge_Send_Dataa, &DJI_Judge_Send_Data->Judge_Send_Data.frame_header, sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header));
	append_crc8_check_sum(Judge_Send_Dataa,sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header));
	
	DJI_Judge_Send_Data->Judge_Send_Data.cmdid = DJI_Judge_Student_CmdID;
	DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data.data_cmd_id = Stu_data_cmd_id;
	DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data.sender_ID = \
	(uint16_t)DJI_Judge_Send_Data->DJI_Judge_Mes.Judge_game_robot_status.robot_id;
	DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data.receiver_ID = Get_Robot_Client_Id(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data.sender_ID);
	
	
	
	memcpy(Judge_Send_Dataa+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header),(uint8_t *)&DJI_Judge_Send_Data->Judge_Send_Data.cmdid,sizeof(DJI_Judge_Send_Data->Judge_Send_Data.cmdid));
	
	memcpy(Judge_Send_Dataa+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header)+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.cmdid),(uint8_t*)&DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data,sizeof(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data)); 	

	memcpy(Judge_Send_Dataa+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header)+sizeof(DJI_Judge_Send_Data->Judge_Send_Data.cmdid) + sizeof(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data),p_data,Data_long); 		


//	memcpy(Judge_Send_Dataa + sizeof(DJI_Judge_Send_Data->Judge_Send_Data.frame_header) + sizeof(DJI_Judge_Send_Data->Judge_Send_Data.Judge_student_interactive_header_data), p_data, sizeof(&p_data));
	append_crc16_check_sum(Judge_Send_Dataa,sizeof(DJI_Judge_Send_Data->Judge_Send_Data) + Data_long);
	Judge_Send_Data_Long = sizeof(DJI_Judge_Send_Data->Judge_Send_Data) + Data_long;

		for(i = 0;i < Judge_Send_Data_Long;i++)
		{
			USART6_SendData(Judge_Send_Dataa[i]);
//			while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
//			USART_SendData(USART6,(uint8_t)Judge_Send_Dataa[i]);			
		}	
	
	Judge_Send_Seq++;
	if(Judge_Send_Seq == 0xFF)
	{
		Judge_Send_Seq = 0;
	}
}


//����ϵͳ����ͼ������ˢ��
void Judge_Graphic_operate_tpye_Set(graphic_data_struct_t* graphic_data_struct_Update,uint8_t* Graphic_Name,Judge_Graphic_Control_t Judge_Graphic_Update)
{
	
	graphic_data_struct_Update->graphic_name[0] = *Graphic_Name;
	graphic_data_struct_Update->graphic_name[1] = *Graphic_Name+1;
	graphic_data_struct_Update->graphic_name[2] = *Graphic_Name+2;
	graphic_data_struct_Update->operate_tpye = Judge_Graphic_Update;
}
//����ϵͳ������������ˢ��
void Judge_Word_operate_tpye_Set(Word_data_struct_t* Word_data_struct_Update,uint8_t* Graphic_Name,Judge_Graphic_Control_t Judge_Graphic_Update)
{
	Word_data_struct_Update->graphic_name[0] = *Graphic_Name;
	Word_data_struct_Update->graphic_name[1] = *Graphic_Name+1;
	Word_data_struct_Update->graphic_name[2] = *Graphic_Name+2;	
	Word_data_struct_Update->operate_tpye = Judge_Graphic_Update;
}

//����ϵͳ�����ַ�������ˢ��
void Judge_Character_operate_tpye_Set(Judge_client_custom_character_t* Judge_client_custom_character_Update,uint8_t* Graphic_Name,Judge_Graphic_Control_t Judge_Graphic_Update)
{
	Judge_client_custom_character_Update->grapic_data_struct.graphic_name[0] = *Graphic_Name;
	Judge_client_custom_character_Update->grapic_data_struct.graphic_name[1] = *Graphic_Name+1;
	Judge_client_custom_character_Update->grapic_data_struct.graphic_name[2] = *Graphic_Name+2;
	Judge_client_custom_character_Update->grapic_data_struct.operate_tpye = Judge_Graphic_Update;
}


int Judge_Data_Update(uint8_t* Judge_Data_Get,DJI_Judge_t* DJI_Judge_Update)
{
		int Judge_Data_Len;
		//У��ͷ֡SOF
		if(Judge_Data_Get[0] == DJI_Judge_Header_SOF)
		{
			//���������ݴ���ṹ��
			DJI_Judge_Update->Judge_Data.frame_header.sof = DJI_Judge_Header_SOF;
			DJI_Judge_Update->Judge_Data.frame_header.data_length = (uint16_t)(Judge_Data_Get[1] | (Judge_Data_Get[2] << 8));
			DJI_Judge_Update->Judge_Data.frame_header.seq = Judge_Data_Get[3];
			//ͷ֡CRC8У���ж�
			if(verify_crc8_check_sum( Judge_Data_Get, Judge_Header_LEN ) == TRUE)
			{
				Judge_Data_Len = DJI_Judge_Update->Judge_Data.frame_header.data_length + Judge_Header_LEN + Judge_Cmd_id_LEN + Judge_CRC16_LEN;
				DJI_Judge_Update->Judge_Data.cmdid = (uint16_t)(Judge_Data_Get[5] | (Judge_Data_Get[6] << 8));
				//β֡CRC16У���ж�
				if(verify_crc16_check_sum( Judge_Data_Get, Judge_Data_Len ) == TRUE)
				{			
					switch(DJI_Judge_Update->Judge_Data.cmdid)
					{
						case Judge_CmdID_Game_State:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_game_status, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Game_State);
							break;
						}
						case Judge_CmdID_Game_Result:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_game_result, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Game_Result);
							break;
						}
						case Judge_CmdID_Robot_Blood:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_game_robot_HP, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Robot_Blood);
							break;
						}
						case Judge_CmdID_Missile_State:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_dart_status, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Missile_State);
							break;
						}
						case Judge_CmdID_Area_Event:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_event_data, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Area_Event);
							break;
						}
						case Judge_CmdID_Area_Supply_Station_Action_Data:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_supply_projectile_action, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Area_Supply_Station_Action_Data);
							break;
						}
						case Judge_CmdID_Waening_Data:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_referee_warning, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Waening_Data);
							break;
						}
						case Judge_CmdID_Missile_Time_Limit_Count:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_dart_remaining_time, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Missile_Time_Limit_Count);
							break;
						}
						case Judge_CmdID_Robot_State:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_game_robot_status, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Robot_State);
							break;
						}
						case Judge_CmdID_Real_Time_Power_Heat:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_power_heat_data, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Real_Time_Power_Heat);
							break;
						}
						case Judge_CmdID_Robot_Position_Data:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_game_robot_pos, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Robot_Position_Data);
							break;
						}
						case Judge_CmdID_Robot_Gain_Data:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_buff, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Robot_Gain_Data);
							break;
						}
						case Judge_CmdID_Aerial_Robot_Power_Data:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_aerial_robot_energy, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Aerial_Robot_Power_Data);
							break;
						}
						case Judge_CmdID_Heat_Data:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_robot_hurt, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Heat_Data);
							break;
						}
						case Judge_CmdID_Real_Time_Shoot_Data:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_shoot_data, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Real_Time_Shoot_Data);
							break;
						}
						case Judge_CmdID_Bullet_Shoot_Limit:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_bullet_remaining, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Bullet_Shoot_Limit);
							break;
						}
						case Judge_CmdID_Robot_RFID_State:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_RFID_status, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Robot_RFID_State);
							break;
						}
						case Judge_CmdID_Missile_Client_Control_Data:
						{
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_dart_client_cmd, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), Judge_Len_Missile_Client_Control_Data);
							break;
						}
						case Judge_CmdID_Robot_Communication_Data:
						{
	//						DJI_Judge_Update->DJI_Judge_Mes.Judge_Student_Data.Judge_Student_Data = Judge_Student_Communicate_Dataa;
							memcpy(&DJI_Judge_Update->DJI_Judge_Mes.Judge_Student_Data, (Judge_Data_Get + Judge_Header_LEN + Judge_Cmd_id_LEN), DJI_Judge_Update->Judge_Data.frame_header.data_length);
							break;
						}	
					
					
					}
					return Judge_Data_Len;	
				}
				return 0;
			}			
			return 0;
		}
		return 0;
}	



void Judge_Data_check(uint8_t* Judge_Data)
{
	static int Judge_Len;
	int i;
		for(i = 0;i<DJI_Judge_Buf_Len_Max;i++)
		{		
			Judge_Len = Judge_Data_Update(Judge_Data+i,&DJI_Judge);	
			if(Judge_Len!= 0)
			{
				i = i+ Judge_Len - 1 ;
			}		
		}
}

//int* 


DJI_Judge_Mes_t* get_Judge_Mes_Add(void)
{
	return &DJI_Judge.DJI_Judge_Mes;
}
