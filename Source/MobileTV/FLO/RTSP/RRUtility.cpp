#if 0 //not used any longer
#include "utility.h"
#include "RRUtility.h"

double rtcp_interval(int members, int senders, double rtcp_bw, int we_sent, double avg_rtcp_size, int initial)
{
	double interval = CUserOptions::UserOptions.m_nRTCPInterval*1.0/1000.0;
	if(interval < 0.5)
		interval = 0.5;

	return interval;
}

void OnExpire(event e, int members, int senders, double rtcp_bw, int we_sent, double *avg_rtcp_size, int *initial, time_tp tc, time_tp *tp, int *pmembers)
{
	double t;     /* Interval */
	double tn;    /* Next transmit time */

	/* 
	if (TypeOfEvent(e) == EVENT_BYE) 
	{
		t = rtcp_interval(members,
			senders,
			rtcp_bw,
			we_sent,
			*avg_rtcp_size,
			*initial);
		tn = *tp + t;
		if (tn <= tc) 
		{
			SendBYEPacket(e);
		} 
		else 
		{
			Schedule(tn, e);
		}

	} 
	else if (TypeOfEvent(e) == EVENT_REPORT) 
	*/
	{
		t = rtcp_interval(members,
			senders,
			rtcp_bw,
			we_sent,
			*avg_rtcp_size,
			*initial);
		tn = *tp + t;

		if (tn <= tc) 
		{
			SendRTCPReport(e);
			//*avg_rtcp_size = (1./16.)*SentPacketSize(e) + (15./16.)*(*avg_rtcp_size);
			*tp = tc;

			t = rtcp_interval(members,
				senders,
				rtcp_bw,
				we_sent,
				*avg_rtcp_size,
				*initial);

			Schedule(t*1000,e);
			*initial = 0;
		}
		else
		{
			Schedule(2000, e);
		}
		*pmembers = members;
	}
}

void OnReceive(packet p,
			   event e,
			   int *members,
			   int *pmembers,
			   int *senders,
			   double *avg_rtcp_size,
			   double *tp,
			   double tc,
			   double tn)
{
	if (PacketType(p) == PACKET_RTCP_REPORT)
	{
		if (NewMember(p) && (TypeOfEvent(e) == EVENT_REPORT))
		{
			AddMember(p);
			*members += 1;
		}
		//*avg_rtcp_size = (1./16.)*ReceivedPacketSize(p) + (15./16.)*(*avg_rtcp_size);
	}
	else if (PacketType(p) == PACKET_RTP)
	{
		if (NewMember(p) && (TypeOfEvent(e) == EVENT_REPORT))
		{
			AddMember(p);
			*members += 1;
		}
		if (NewSender(p) && (TypeOfEvent(e) == EVENT_REPORT))
		{
			AddSender(p);
			*senders += 1;
		}
	}
}

#endif