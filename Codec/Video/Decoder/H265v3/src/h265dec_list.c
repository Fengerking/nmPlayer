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

#if USE_CODEC_NEW_API

static VO_S32 UpdateAutoFastMode(H265_DEC* const p_dec, const H265_DPB_LIST * const p_dpb_list)
{
	VO_S32 fastmode = p_dec->fast_mode;
	if (fastmode & VO_FM_AUTO_FASTMODE)
	{
		const VO_S32 nOutNum = p_dec->frame_num;
		fastmode = VO_FM_AUTO_FASTMODE; //reset

		if (nOutNum > 50)
		{
			const VO_S32 nListCount = p_dpb_list->disListLeft - 1; //empty num of dis list
			if (nListCount > 0)
			{
				const VO_S32 nListSize = p_dpb_list->node_num - 1; //total size of dis list size

				if (nListSize <= AUTO_FASTMODE_DROP*nListCount ) //50% empty
				{
					fastmode |= VO_FM_DROP_UNREF;
				}

				if (nListSize <= AUTO_FASTMODE_DBK*nListCount ) //33% empty
				{
					fastmode |= VO_FM_DEBLOCK;
				}

				if (nListSize <= AUTO_FASTMODE_SAO*nListCount) //20% empty
				{
					fastmode |= VO_FM_SAO;
				}
			}
		}

		p_dec->fast_mode = fastmode;
		if (p_dec->nThdNum <= 1)
		{
			p_dec->slice.fast_mode = fastmode;
		}
	}

	return fastmode;
}
#endif

/* Display picture FIFO*/
VO_S32  PushDisPic(H265_DEC* const p_dec, H265_DPB_LIST *p_dpb_list, TComPic*  p_dis_pic)
{
	H265_LIST_NODE * p_tail = p_dpb_list->dis_tail;
	H265_LIST_NODE * const p_base = p_dpb_list->dis_node_pool;
	H265_LIST_NODE * p_next_tail = p_base + p_tail->next;
	
	if (p_next_tail->data != NULL)
	{
		//dis_list is full, we should not input frame to decoder
		VOASSERT(p_dpb_list->disListLeft == 0);
		p_dec->bOutputFull = 1;
// printf("disable\n");
		return 1;
	}

	assert(p_tail->data == NULL && p_dpb_list->disListLeft > 0);
	p_tail->data = p_dis_pic;
	p_dpb_list->dis_tail = p_next_tail;//tail is always empty;
  p_dpb_list->m_iPOCLastDisplay = p_dis_pic->m_iPOC;
	p_dpb_list->disListLeft--;
LOGI("POC %ld add to display list\n", p_dis_pic->m_iPOC);
	return 0;
}

TComPic*  PopDisPic(H265_DEC *p_dec, H265_DPB_LIST *p_dpb_list, VO_S32* next_pic_ready)
{
	H265_LIST_NODE * p_head = p_dpb_list->dis_head;
	H265_LIST_NODE * const p_base = p_dpb_list->dis_node_pool;

	*next_pic_ready = 0;
	if (p_head && p_head->data != NULL) {
		TComPic*  p_dis_pic = p_head->data;

#if USE_3D_WAVE_THREAD
		if (p_dec->nThdNum > 1)
		{
			pthread_mutex_t *outputMutex = &p_dec->pWave3DThdPool->outputMutex;
			pthread_mutex_lock(outputMutex);
			while (p_dis_pic && p_dis_pic->errFlag) //find first no-error pic
			{
LOGI("remove first err POC %ld, dis_list %ld\n", p_dis_pic->m_iPOC, p_dpb_list->disListLeft+1);
				p_head->data = NULL;// empty node
				p_dpb_list->dis_head = p_head = p_base + p_head->next;//next node
				p_dis_pic = p_head->data;
				p_dpb_list->disListLeft++;
				p_dec->bOutputFull = 0;
// printf("enable\n");
			}
			if (p_dis_pic)
			{
				if (!p_dis_pic->bFinishDecode)
				{
//LOGI("next output POC %d\n", p_dis_pic->m_iPOC);
					*next_pic_ready |= 1;
// 					p_dis_pic->m_bNeededForOutput = FALSE; //Harry: TBD??????
#if !USE_CODEC_NEW_API
					do 
					{
						p_head->data->m_bNeededForOutput = VO_FALSE; //Harry: TBD??????
// 						printf("output %d\n", p_head->data->m_iPOC);
						p_head = p_base + p_head->next;
					} while (p_head && p_head->data != NULL); //check if there are extra pics follow it
#endif

					pthread_mutex_unlock(outputMutex);
					return NULL;
				}
			}
			else
			{
				pthread_mutex_unlock(outputMutex);
				return NULL;
			}
			pthread_mutex_unlock(outputMutex);
		}
#endif
LOGI("Output POC %ld, dis_list %ld\n", p_dis_pic->m_iPOC, p_dpb_list->disListLeft+1);
		p_head->data = NULL;// empty node
		p_dpb_list->dis_head = p_head = p_base + p_head->next;//next node
		p_dpb_list->disListLeft++;
		p_dec->bOutputFull = 0;
		p_dec->frame_num++;		//count the output frame num
// printf("enable\n");
#if USE_CODEC_NEW_API
		UpdateAutoFastMode(p_dec, p_dpb_list);
#endif
		if (p_dec->nThdNum > 1)
		{
			TComPic*  p_next_pic = p_head->data;
			pthread_mutex_t *outputMutex = &p_dec->pWave3DThdPool->outputMutex;
			pthread_mutex_lock(outputMutex);
			while (p_next_pic && p_next_pic->errFlag) //find first no-error pic
			{
LOGI("remove next err POC %ld, dis_list %ld\n", p_next_pic->m_iPOC, p_dpb_list->disListLeft+1);
				p_head->data = NULL;// empty node
				p_dpb_list->dis_head = p_head = p_base + p_head->next;//next node
				p_next_pic = p_head->data;
				p_dpb_list->disListLeft++;
				p_dec->bOutputFull = 0;
				// printf("enable\n");
			}
			pthread_mutex_unlock(outputMutex);
		}
		if (p_head->data != NULL) {//another pic is ready
LOGI("next output POC %ld\n", p_head->data->m_iPOC);
			*next_pic_ready |= 1;
		}

		return p_dis_pic;
	}
	p_dec->bOutputFull = 0;

	return NULL; //no dis play pic
}


VO_VOID CreateDpbList(H265_DPB_LIST *p_dpb_list, TComPic* dpb_pool)
{
	VO_U32 i;
	H265_LIST_NODE * const p_base = p_dpb_list->list_node_pool;
	H265_LIST_NODE * const p_base_dis = p_dpb_list->dis_node_pool;
	H265_LIST_NODE* p_cur_node = p_base;//node[0]
	VO_U32 node_num = p_dpb_list->node_num;

	/////////////dbp list
	/* create list,  bi-direction circulation list*/
	for(i = 0; i < node_num; i++){
		p_cur_node->previous = i - 1;
		p_cur_node->data = dpb_pool++;
		p_cur_node->next = i + 1;
		p_cur_node++;
	}
	p_base->previous            = node_num - 1;
	p_dpb_list->list_node_pool[node_num - 1].next = 0;

	/*ini list*/ 
	p_dpb_list->size = 0;
	p_dpb_list->real_size = 0;
	p_dpb_list->tail = p_dpb_list->head = NULL;

	/*Ini fifo*/
	p_dpb_list->p_fifo  = &p_dpb_list->list_node_pool[0];


	/////////////display list
	p_cur_node = p_base_dis;
	/*Ini display list*/

#if !USE_CODEC_NEW_API
	--node_num;
#endif
	/* create list,  bi-direction circulation list*/
	for(i = 0; i < node_num; i++){
		p_cur_node->previous = i - 1;
		p_cur_node->data = NULL;
		p_cur_node->next = i + 1;
		p_cur_node++;
	}
	p_base_dis->previous            = node_num - 1;
	p_dpb_list->dis_node_pool[node_num - 1].next = 0;

	/*ini dis list*/ 
	p_dpb_list->dis_tail = p_dpb_list->dis_head = p_base_dis;
	p_dpb_list->m_iPOCLastDisplay = -VO_MAX_INT; 
	p_dpb_list->disListLeft = (VO_S32)node_num;
}

//clear dpb list, but except p_dpb_list->head. 
//Because IDR frame should be kept in p_dpb_list
VO_VOID ClearDpbList(H265_DPB_LIST *p_dpb_list)
{
#if USE_CODEC_NEW_API
	if (p_dpb_list->tail)
	{
		H265_LIST_NODE* node = p_dpb_list->real_head;
		while (node != p_dpb_list->tail)
		{
			node->data->m_bRefenced = VO_FALSE;
			node = p_dpb_list->list_node_pool + node->next;
		}
		//for cur frame is already get, so exclude it
		p_dpb_list->head = p_dpb_list->tail;
		p_dpb_list->size = 1;
	}
	else
	{
		p_dpb_list->head = NULL;
	p_dpb_list->size = 0;
	}
#else
	p_dpb_list->tail = p_dpb_list->head = NULL;
	//p_dpb_list->p_fifo  = &p_dpb_list->list_node_pool[0];
	p_dpb_list->size = 0;
#endif
// printf("clean dpb\n");
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
VO_VOID xWriteOutput(H265_DEC * const p_dec, H265_DPB_LIST* pcListPic, VO_S32 reorder_num )
{
	H265_LIST_NODE* const p_base   = pcListPic->list_node_pool;
	H265_LIST_NODE* p_head   = pcListPic->head;
	VO_U32 list_size         = pcListPic->size;
// 	H265_DEC_SLICE *p_slice  = &p_dec->slice;//YU_TBD multi
	VO_S32 not_displayed     = 0;
	//VO_S32 m_numReorderPics  = p_slice->p_sps->sps_max_num_reorder_pics[tId];

	if (list_size == 0) //no display pic
		return ;

#if USE_CODEC_NEW_API
	if (reorder_num >= 0)
	{
		list_size--; //for new frame already get, so exclude it
	}
	else
	{
		VOASSERT(reorder_num == -1);
		reorder_num = 0;
	}
#endif

// printf("before output size %d\n", list_size);
	while (list_size) {
		TComPic* pcPic = p_head->data;
	
		if (pcPic->m_bNeededForOutput == VO_TRUE && (pcPic->m_iPOC > pcListPic->m_iPOCLastDisplay)) {
			not_displayed++;
		}
		p_head = p_base + p_head->next;
		list_size--;
	}


	p_head    = pcListPic->head;
	list_size = pcListPic->size;


	not_displayed -= reorder_num;
// #if USE_CODEC_NEW_API
// 	if (not_displayed <= 0)
// 	{
// 		p_dec->bOutputFull = 0;
// 		pthread_cond_signal(&p_dec->disListCond);
// 		return;
// 	}
// #endif

	pthread_mutex_lock(&p_dec->disListMutex);
	while (list_size && (not_displayed > 0)) {
		TComPic* pcPic = p_head->data;
		
		if (pcPic->m_bNeededForOutput == VO_TRUE)
		{
			if (pcPic->m_iPOC > pcListPic->m_iPOCLastDisplay)
			{
			if (PushDisPic(p_dec, pcListPic, pcPic))
			{
//if (reorder_num == -1)
//{
//	LOGI("flush POC %d failed\n", pcPic->m_iPOC);
//}
				break;
			}
				pcPic->m_bNeededForOutput = PIC_READY_FOR_OUT;
			// write to file
			not_displayed--;
		}
			else
			{
				//for stability
				pcPic->m_bNeededForOutput = VO_FALSE;
			}
		}

// 		if ( pcPic->m_bNeededForOutput == VO_TRUE && (pcPic->m_iPOC > pcListPic->m_iPOCLastDisplay)) {
// 			if (PushDisPic(p_dec, pcListPic, pcPic))
// 			{
// // if (reorder_num == -1)
// // {
// // 	printf("flush POC %d failed\n", pcPic->m_iPOC);
// // }
// 				break;
// 			}
// 			pcPic->m_bNeededForOutput = PIC_READY_FOR_OUT;
// 			// write to file
// 			not_displayed--;
// 		}
		p_head = p_base + p_head->next;
		list_size--;
	}
	pthread_mutex_unlock(&p_dec->disListMutex);
	pthread_cond_signal(&p_dec->disListCond);
}
