 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/*  \file     h265dec_list.c
    \brief    list for the pic
    \author   Jo Liu
	  \change
*/


#include "h265dec_list.h"



/* Display picture FIFO*/
VO_VOID  PushDisPic(H265_DPB_LIST *p_dpb_list, TComPic*  p_dis_pic)
{
	H265_LIST_NODE * p_tail = p_dpb_list->dis_tail;
	H265_LIST_NODE * const p_base = p_dpb_list->dis_node_pool;
	
	p_tail->data = p_dis_pic;
	p_dpb_list->dis_tail = p_base + p_tail->next;//tail is always empty;
  p_dpb_list->m_iPOCLastDisplay = p_dis_pic->m_iPOC;
}

TComPic*  PopDisPic(H265_DEC *p_dec, H265_DPB_LIST *p_dpb_list, VO_S32* next_pic_ready)
{
	H265_LIST_NODE * p_head = p_dpb_list->dis_head;
	H265_LIST_NODE * const p_base = p_dpb_list->dis_node_pool;

	*next_pic_ready = 0;
	if (p_head && p_head->data != NULL) {
		TComPic*  p_dis_pic = p_head->data;

#if USE_FRAME_THREAD
		if (p_dec->nThdNum > 1)
		{
			pthread_mutex_lock(&p_dec->pFrameThdPool->outputMutex);
			while (p_dis_pic && p_dis_pic->errFlag) //find first no-error pic
			{
				p_head->data = NULL;// empty node
				p_dpb_list->dis_head = p_head = p_base + p_head->next;//next node
				p_dis_pic = p_head->data;
			}
			if (p_dis_pic)
			{
				if (!p_dis_pic->bFinishDecode)
				{
					*next_pic_ready |= 1;
// 					p_dis_pic->m_bNeededForOutput = FALSE; //Harry: TBD??????

					do 
					{
						p_head->data->m_bNeededForOutput = FALSE; //Harry: TBD??????
// 						printf("output %d\n", p_head->data->m_iPOC);
						p_head = p_base + p_head->next;
					} while (p_head && p_head->data != NULL); //check if there are extra pics follow it

					pthread_mutex_unlock(&p_dec->pFrameThdPool->outputMutex);
					return NULL;
				}
			}
			else
			{
				pthread_mutex_unlock(&p_dec->pFrameThdPool->outputMutex);
				return NULL;
			}
			pthread_mutex_unlock(&p_dec->pFrameThdPool->outputMutex);
		}
#endif
		p_head->data = NULL;// empty node
		p_dpb_list->dis_head = p_head = p_base + p_head->next;//next node

		if (p_head->data != NULL) {//another pic is ready
			*next_pic_ready |= 1;
		}

		return p_dis_pic;
	}

	return NULL; //no dis play pic
}


VO_VOID CreateDpbList(H265_DPB_LIST *p_dpb_list, TComPic* dpb_pool)
{
	VO_U32 i;
	H265_LIST_NODE * const p_base = p_dpb_list->list_node_pool;
	H265_LIST_NODE * const p_base_dis = p_dpb_list->dis_node_pool;
	H265_LIST_NODE* p_cur_node = p_base;//node[0]

	/////////////dbp list
	/* create list,  bi-direction circulation list*/
	for(i = 0; i < p_dpb_list->node_num; i++){
		p_cur_node->previous = i - 1;
		p_cur_node->data = dpb_pool++;
		p_cur_node->next = i + 1;
		p_cur_node++;
	}
	p_base->previous            = p_dpb_list->node_num - 1;
	p_dpb_list->list_node_pool[p_dpb_list->node_num - 1].next = 0;

	/*ini list*/ 
	p_dpb_list->size = 0;
	p_dpb_list->tail = p_dpb_list->head = NULL;

	/*Ini fifo*/
	p_dpb_list->p_fifo  = &p_dpb_list->list_node_pool[0];


	/////////////display list
	p_cur_node = p_base_dis;
	/*Ini display list*/
	/* create list,  bi-direction circulation list*/
	for(i = 0; i < p_dpb_list->node_num; i++){
		p_cur_node->previous = i - 1;
		p_cur_node->data = NULL;
		p_cur_node->next = i + 1;
		p_cur_node++;
	}
	p_base_dis->previous            = p_dpb_list->node_num - 1;
	p_dpb_list->dis_node_pool[p_dpb_list->node_num - 1].next = 0;

	/*ini dis list*/ 
	p_dpb_list->dis_tail = p_dpb_list->dis_head = p_base_dis;
	p_dpb_list->m_iPOCLastDisplay = -VO_MAX_INT; 
}

//clear dpb list, but except p_dpb_list->head. 
//Because IDR frame should be kept in p_dpb_list
VO_VOID ClearDpbList(H265_DPB_LIST *p_dpb_list)
{
	p_dpb_list->size = 0;
	p_dpb_list->tail = p_dpb_list->head = NULL;
	//p_dpb_list->p_fifo  = &p_dpb_list->list_node_pool[0];

    p_dpb_list->m_iPOCLastDisplay = -VO_MAX_INT;

}

VO_VOID sortPicList(H265_DPB_LIST* p_dpb_list)
{
	VO_S32 i;//, j;
    H265_LIST_NODE *p_tail = p_dpb_list->tail;	
	H265_LIST_NODE * const p_base = p_dpb_list->list_node_pool;	

	for ( i = 1; i < (VO_S32)p_dpb_list->size; i++) {
		//for (j = i; j < (VO_S32)p_dpb_list->size; j++) {
			TComPic* p_tail_pic = p_tail->data;
			TComPic* p_previous_pic = (p_base + p_tail->previous)->data;

			if (p_tail_pic->m_iPOC >= p_previous_pic->m_iPOC)
				break;

			//if (p_tail_pic->m_iPOC < p_previous_pic->m_iPOC ) {//swap
				p_tail->data = p_previous_pic;
				(p_base + p_tail->previous)->data = p_tail_pic;
			//}
			p_tail = p_base + p_tail->previous;
			
		//}
	}

#if 0	
		for (j = 0; j < i; j++) 
			iterPicExtract++;

		pcPicExtract = *(iterPicExtract);
		pcPicExtract->setCurrSliceIdx(0);
		iterPicInsert = rcListPic.begin();

		while (iterPicInsert != iterPicExtract) {
			pcPicInsert = *(iterPicInsert);
			pcPicInsert->setCurrSliceIdx(0);
			if (pcPicInsert->m_iPOC >= pcPicExtract->m_iPOC) {
				break;
			}
			iterPicInsert++;
		}
    
		iterPicExtract_1 = iterPicExtract;    iterPicExtract_1++;
    
		//  swap iterPicExtract and iterPicInsert, iterPicExtract = curr. / iterPicInsert = insertion position
		rcListPic.insert (iterPicInsert, iterPicExtract, iterPicExtract_1);
		rcListPic.erase  (iterPicExtract);
	}
#endif

}



/** \param pcListPic list of pictures to be written to file
    \todo            DYN_REF_FREE should be revised
 */
VO_VOID xWriteOutput(/*H265_DEC *p_dec,*/ H265_DPB_LIST* pcListPic, VO_S32 reorder_num )
{
	H265_LIST_NODE* const p_base   = pcListPic->list_node_pool;
	H265_LIST_NODE* p_head   = pcListPic->head;
	VO_U32 list_size         = pcListPic->size;
// 	H265_DEC_SLICE *p_slice  = &p_dec->slice;//YU_TBD multi
	VO_S32 not_displayed     = 0;
	//VO_S32 m_numReorderPics  = p_slice->p_sps->sps_max_num_reorder_pics[tId];

	if (list_size == 0) //no display pic
		return ;


	while (list_size) {
		TComPic* pcPic = p_head->data;
	
		if (pcPic->m_bNeededForOutput && (pcPic->m_iPOC > pcListPic->m_iPOCLastDisplay)) {
			not_displayed++;
		}
		p_head = p_base + p_head->next;
		list_size--;
	}


	p_head    = pcListPic->head;
	list_size = pcListPic->size;


	not_displayed -= reorder_num;
	while (list_size && (not_displayed > 0)) {
		TComPic* pcPic = p_head->data;
		
		if ( pcPic->m_bNeededForOutput && (pcPic->m_iPOC > pcListPic->m_iPOCLastDisplay)) {
			// write to file
			not_displayed--;

			PushDisPic(pcListPic, pcPic);			
		}
		p_head = p_base + p_head->next;
		list_size--;
	}
}
