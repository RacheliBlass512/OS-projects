#include "os.h"


uint64_t check_for_mapping(uint64_t* start_point,  int index){
	if(!(start_point[index]&1)){
		return NO_MAPPING;
	}
	return start_point[index];
}

uint64_t allocates_new_level(uint64_t* start_point, int index){
	uint64_t pt = alloc_page_frame();
	uint64_t line_content=pt<<12;
	line_content=line_content | 1;
	start_point[index]= line_content;
	return line_content;
}

void zero_bit_V(uint64_t* start_point, int index){
	uint64_t temp= start_point[index];
	temp=temp>>1;
	temp=temp<<1;
	start_point[index]=temp;
	return;
}

void update_line_content(uint64_t* start_point, int index, uint64_t new_content){
	uint64_t* ptr = start_point+index*8;
	*ptr = new_content;
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
	int devided_to_five[5]; 
	uint64_t temp= 0x1ff;
	for(int i=4; i>=0;i--){
		devided_to_five[i] = (vpn & temp)>>(4-i)*9;
		temp=temp<<9;
	}
	pt=pt<<12;
	uint64_t* vir_pt;
	uint64_t line_content;
	if(ppn==NO_MAPPING){
		for(int i=0;i<4;i++){
			vir_pt = phys_to_virt(pt);
			line_content = check_for_mapping(vir_pt, devided_to_five[i]);
			if(line_content==NO_MAPPING){
				return;
			}
			pt=line_content;
		}
		vir_pt = phys_to_virt(pt);
		zero_bit_V(vir_pt, devided_to_five[4]);
		return;
	}
	for(int i=0;i<4;i++){
		vir_pt = phys_to_virt(pt);
		line_content = check_for_mapping(vir_pt, devided_to_five[i]);
		if(line_content==NO_MAPPING){
			uint64_t new_line_content = allocates_new_level(vir_pt, devided_to_five[i]);
			pt = new_line_content;
		}
		else{
			pt=line_content;
		}
	}
	vir_pt = phys_to_virt(pt);
	vir_pt[devided_to_five[4]]=ppn;
	return;
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
	int devided_to_five[5]; 
	uint64_t temp= 0x1ff;
	for(int i=4; i>=0;i--){
		devided_to_five[i] = (vpn & temp)>>(4-i)*9;
		temp=temp<<9;
	}
	pt=pt<<12;
	uint64_t line_content;
	uint64_t* vir_pt;
	for(int i=0;i<5;i++){
		vir_pt = phys_to_virt(pt);
		line_content = check_for_mapping(vir_pt, devided_to_five[i]);
		if(line_content == NO_MAPPING){
			return NO_MAPPING;
		}
		pt = line_content;
	}
	return line_content;
}