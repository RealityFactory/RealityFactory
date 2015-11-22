
#include "RabidFramework.h"

CTrack::CTrack()
{
	TrackCount=0;
}

CTrack::~CTrack()
{
}

//=====================================================================================
//	Track_OnTrack
//=====================================================================================
geBoolean CTrack::Track_OnTrack(TrackData *td)
{
    return (td->TrackNdx >= 0);
}

//=====================================================================================
//	Track_GetNextTrack
//=====================================================================================
Track* CTrack::Track_GetNextTrack(Track *t)
{
	int32 ndx;

	if (!t)
		return(&TrackList[0]);

	ndx = t - &TrackList[0];
	assert(ndx >= 0 && ndx < TrackCount);
	ndx++;

	if (ndx >= TrackCount)
		return NULL;

	return (&TrackList[ndx]);
}

//=====================================================================================
//	Track_ClearTrack
//=====================================================================================
void CTrack::Track_ClearTrack(TrackData *td)
{
    td->TrackNdx = -1;
    td->PointNdx = -1;
}

//=====================================================================================
//	Track_GetTrack
//=====================================================================================
Track *CTrack::Track_GetTrack(TrackData *td)
{
	return &TrackList[td->TrackNdx];
}

//=====================================================================================
//	Track_GetDir
//=====================================================================================
int32 CTrack::Track_GetDir(TrackData *td)
{
	return td->TrackDir;
}

//=====================================================================================
//	Track_PastStartPoint
//=====================================================================================
geBoolean CTrack::Track_PastStartPoint(TrackData *td)
{
	assert(td->TrackNdx > -1);

	if (td->TrackDir == 1)
	{
		return (td->PointNdx > 0);
	}
	else
	if (td->TrackDir == -1)
	{
		return (td->PointNdx < TrackList[td->PointNdx].PointCount);
	}

	assert(1==0);
	return GE_FALSE;
}

//=====================================================================================
//	Track_NextPoint
//=====================================================================================
TrackPt *CTrack::Track_NextPoint(TrackData *td)
{
	assert(td->TrackNdx > -1);

    td->PointNdx += td->TrackDir;

    if (td->PointNdx >= TrackList[td->TrackNdx].PointCount)
        {
        if (TEST(TrackList[td->TrackNdx].Flags, TRACK_FLAG_LOOP))
            {
            td->PointNdx = 0;
            return(&TrackList[td->TrackNdx].PointList[td->PointNdx]);
            }

        return(NULL);
        }

    if (td->PointNdx < 0)
        {
        if (TEST(TrackList[td->TrackNdx].Flags, TRACK_FLAG_LOOP))
            {
            td->PointNdx = TrackList[td->TrackNdx].PointCount - 1;
            return(&TrackList[td->TrackNdx].PointList[td->PointNdx]);
            }

		td->PointNdx = 0;
        return(NULL);
        }

    assert(td->PointNdx < TrackList[td->TrackNdx].PointCount);

    return(&TrackList[td->TrackNdx].PointList[td->PointNdx]);
}


//=====================================================================================
//	Track_PrevPoint
//=====================================================================================
TrackPt *CTrack::Track_PrevPoint(TrackData *td)
    {
	assert(td->TrackNdx > -1);

    td->PointNdx -= td->TrackDir;

    if (td->PointNdx >= TrackList[td->TrackNdx].PointCount)
        {
        if (TEST(TrackList[td->TrackNdx].Flags, TRACK_FLAG_LOOP))
            {
            td->PointNdx = 0;
            return(&TrackList[td->TrackNdx].PointList[td->PointNdx]);
            }

        return(NULL);
        }

    if (td->PointNdx < 0)
        {
        if (TEST(TrackList[td->TrackNdx].Flags, TRACK_FLAG_LOOP))
            {
            td->PointNdx = TrackList[td->TrackNdx].PointCount - 1;
            return(&TrackList[td->TrackNdx].PointList[td->PointNdx]);
            }

        return(NULL);
        }

    assert(td->PointNdx < TrackList[td->TrackNdx].PointCount);

    return(&TrackList[td->TrackNdx].PointList[td->PointNdx]);
    }


//=====================================================================================
//	Track_GetPoint
//=====================================================================================
TrackPt *CTrack::Track_GetPoint(TrackData *td)
    {
	assert(td);

    assert(Track_OnTrack(td));

    if (td->PointNdx >= TrackList[td->TrackNdx].PointCount)
        td = td;

    assert(td->PointNdx < TrackList[td->TrackNdx].PointCount);

	return &TrackList[td->TrackNdx].PointList[td->PointNdx];
    }

//=====================================================================================
//	Track_Length
//=====================================================================================
float CTrack::Track_Length(Track* t)
{
	TrackPt *tp;
	float dist=0.0f;

    for (tp = t->PointList; tp < &t->PointList[t->PointCount-1]; tp++)
	{
		dist += geVec3d_DistanceBetween(tp->Pos, (tp+1)->Pos);
	}

	return dist;
}

//=====================================================================================
//	Track_GetEndPoint
//=====================================================================================
TrackPt* CTrack::Track_GetEndPoint(TrackData *td)
{
	Track *t = &TrackList[td->TrackNdx];

	assert(td);
    assert(Track_OnTrack(td));

	if (td->TrackDir == 1)
		return(&t->PointList[t->PointCount-1]);
	else
		return(&t->PointList[0]);
}

//=====================================================================================
//	Track_GetFirstPoint
//=====================================================================================
TrackPt* CTrack::Track_GetFirstPoint(TrackData *td)
{
	Track *t = &TrackList[td->TrackNdx];

	assert(td);
    assert(Track_OnTrack(td));

	if (td->TrackDir == -1)
		return(&t->PointList[t->PointCount-1]);
	else
		return(&t->PointList[0]);
}

//=====================================================================================
//	Track_LinkTest
//=====================================================================================
geBoolean CTrack::Track_LinkTest(TrackPt *tp, TrackPt *tp2)
{

	if (geVec3d_DistanceBetween(tp->Pos, tp2->Pos) > 1100)
		return GE_FALSE;

	if (fabs(tp->Pos->Y - tp2->Pos->Y) > TRACK_SEARCH_Y_RANGE * 2)
		return GE_FALSE;

	if (CanSeePointToPoint(tp->Pos, tp2->Pos))
		return GE_TRUE;

	return GE_FALSE;
}

//=====================================================================================
//	Track_LinkSet
//=====================================================================================
int32 CTrack::Track_LinkSet(Track *t, Track *t2)
{
	int32 i=0;

	for (i = 0; i < MAX_TRACK_VIS; i++)
	{
		if (t->Vis[i] == t2)
			break;

		// make sure a spot is open 
		if (!t->Vis[i])
		{
			t->Vis[i] = t2;
			return i;
		}
	}

	assert (i < MAX_TRACK_VIS);

	return -1;
}

//=====================================================================================
//	Track_LinkExists
//=====================================================================================
geBoolean CTrack::Track_LinkExists(Track *t, Track *t2)
{
	int32 i;

	for (i = 0; i < MAX_TRACK_VIS; i++)
	{
		if (t->Vis[i] == t2)
			return GE_TRUE;
	}

	return(GE_FALSE);
}


//=====================================================================================
//	Track_IsOneWay
//=====================================================================================
geBoolean CTrack::Track_IsOneWay(Track *t)
{
    switch (t->Type)
        {
    // for ladders only look at first track point
	case TRACK_TYPE_ELEVATOR_UP:
	case TRACK_TYPE_ELEVATOR_DOWN:
    case TRACK_TYPE_UP:
    case TRACK_TYPE_DOWN:
    case TRACK_TYPE_SCAN:
    case TRACK_TYPE_TRAVERSE_ONEWAY:
            {
			return GE_TRUE;
            }
		}

	return GE_FALSE;
}

//=====================================================================================
//	Track_LinkTrack
//      Large and somewhat nasty.  Every attempt to simplify this didn't really
//      work very well.  Its mostly due to the arbitrary nature that tracks can be
//      placed  Every situation needs to be accounted for.
//
//     E<-----S  E<---->S   Left
//     S----->E  E<---->S	Right
//     E<-----S  S<---->E	Left
//     S----->E  S<---->E	Right
//
//     E<-----S  E<-----S	Left
//     S----->E  E<-----S	NA
//     E<-----S  S----->E	NA
//     S----->E  S----->E	Right
//
//     E<---->S  E<-----S   Left
//     E<---->S	 S----->E   Right
//     S<---->E	 E<-----S   Left
//     S<---->E	 S----->E   Right
//
//        *      E<-----S	Left
//        *      E<-----S	Left
//        *      S----->E	Right
//        *      S----->E	Right
//
//     E<---->S  E<---->S   Left OR Right
//     S<---->E  E<---->S	Left OR Right
//     E<---->S  S<---->E	Left OR Right
//     S<---->E  S<---->E	Left OR Right
//=====================================================================================

geBoolean CTrack::Track_LinkTrack(Track* t, Track *t2)
    {
    TrackPt *tp, *tp2;
	geBoolean OneWay, OneWay2;
	geBoolean a=GE_FALSE,b=GE_FALSE,c=GE_FALSE,d=GE_FALSE;
	int32 *VisFlag, *VisFlag2, i;

	int32 ep, ep2;
    geBoolean OnePoint = GE_FALSE, OnePoint2 = GE_FALSE;
    float e2e, s2s, e2s, s2e;

    if (t == t2)
		return GE_FALSE;

	OneWay = Track_IsOneWay(t);
	OneWay2 = Track_IsOneWay(t2);

    tp = t->PointList;
	ep = t->PointCount - 1;
	tp2 = t2->PointList;
	ep2 = t2->PointCount - 1;

	VisFlag = &t->VisFlag[0];
	VisFlag2 = &t2->VisFlag[0];

    if (ep == 0) OnePoint = GE_TRUE;
    if (ep2 == 0) OnePoint2 = GE_TRUE;

    if (OnePoint && OnePoint2)
        {
        // both single points
		if (!Track_LinkExists(t, t2) &&
            Track_LinkTest(&t->PointList[0], &t2->PointList[0]))
            {
            if ((i = Track_LinkSet(t, t2)) > -1)
				VisFlag[i] |= 1;
            return GE_TRUE;
            }
        }
    else
    if (OnePoint)
        {
        if (OneWay2)
            {
            // first track is single, second is oneway and not single
            s2s = DistWeightedY(t->PointList[0].Pos, t2->PointList[0].Pos, 2.0f);
            s2e = DistWeightedY(t->PointList[0].Pos, t2->PointList[ep2].Pos, 2.0f);

            if (s2s < s2e)
                {
			    if (!Track_LinkExists(t, t2) &&
                    Track_LinkTest(&t->PointList[0], &t2->PointList[0]))
                    {
                    if ((i = Track_LinkSet(t, t2)) > -1)
	                    VisFlag[i] |= 1;
                    return GE_TRUE;
                    }
                }
            else
		    if (!Track_LinkExists(t2, t) &&
                Track_LinkTest(&t->PointList[0], &t2->PointList[ep2]))
                {
                if ((i = Track_LinkSet(t2, t)) > -1)
	                VisFlag2[i] |= 2;
                return GE_TRUE;
                }
            }
        else 
            {
            // onepoint and 2way
		    if (!Track_LinkExists(t, t2) &&
                (a = Track_LinkTest(&t->PointList[0], &t2->PointList[0])) ||
                (b = Track_LinkTest(&t->PointList[0], &t2->PointList[ep2])))
                {
                if ((i = Track_LinkSet(t, t2)) > -1)
					VisFlag[i] |= 1;
                if ((i = Track_LinkSet(t2, t)) > -1)
					VisFlag2[i] |= 2;
				return GE_TRUE;
                }
            }
        }
    else
    if (OnePoint2)
        {
        if (OneWay)
            {
            // second track is single, first is oneway and not single
            s2s = DistWeightedY(t->PointList[0].Pos, t2->PointList[0].Pos, 2.0f);
            e2s = DistWeightedY(t->PointList[ep].Pos, t2->PointList[0].Pos, 2.0f);

            if (e2s < s2s)
                {
		        if (!Track_LinkExists(t, t2) &&
                    Track_LinkTest(&t->PointList[ep], &t2->PointList[0]))
                    {
                    if ((i = Track_LinkSet(t, t2)) > -1)
	                    VisFlag[i] |= 4;
                    return GE_TRUE;
                    }
                }
            else
		    if (!Track_LinkExists(t2, t) &&
                Track_LinkTest(&t->PointList[0], &t2->PointList[0]))
                {
                if ((i = Track_LinkSet(t2, t)) > -1)
	                VisFlag2[i] |= 1;
                return GE_TRUE;
                }
            }
        else
            {
            // 2way and onepoint2
		    if (!Track_LinkExists(t, t2) &&
                (a = Track_LinkTest(&t->PointList[0], &t2->PointList[0])) ||
                (b = Track_LinkTest(&t->PointList[ep], &t2->PointList[0])))
                {
                if ((i = Track_LinkSet(t, t2)) > -1)
					VisFlag[i] |= 1;
                if ((i = Track_LinkSet(t2, t)) > -1)
					VisFlag2[i] |= 4;
                return GE_TRUE;
                }
            }
        }
    else
    if (OneWay || OneWay2)
        {
        e2e = DistWeightedY(t->PointList[ep].Pos, t2->PointList[ep2].Pos, 2.0f);
        s2s = DistWeightedY(t->PointList[0].Pos, t2->PointList[0].Pos, 2.0f);
        s2e = DistWeightedY(t->PointList[0].Pos, t2->PointList[ep2].Pos, 2.0f);
        e2s = DistWeightedY(t->PointList[ep].Pos, t2->PointList[0].Pos, 2.0f);

        if (e2e < s2s && e2e < s2e && e2e < e2s)
            {
            //e2e closest
            if (OneWay && OneWay2)
                return GE_TRUE;

            if (OneWay)
                {
				if (!Track_LinkExists(t, t2) &&
                    Track_LinkTest(&t->PointList[ep], &t2->PointList[ep2]))
                    {
                    if ((i = Track_LinkSet(t, t2)) > -1)
						VisFlag[i] |= 3;
                    return GE_TRUE;
                    }
                }

            if (OneWay2)
                {
				if (!Track_LinkExists(t2, t) &&
                    Track_LinkTest(&t->PointList[ep], &t2->PointList[ep2]))
                    {
                    if ((i = Track_LinkSet(t2, t)) > -1)
	                    VisFlag2[i] |= 3;
                    return GE_TRUE;
                    }
                }
            }


        if (s2s < e2e && s2s < s2e && s2s < e2s)
            {
            //s2s closest
            if (OneWay && OneWay2)
                return GE_TRUE;

            if (OneWay2)
                {
				if (!Track_LinkExists(t, t2) &&
                    Track_LinkTest(&t->PointList[0], &t2->PointList[0]))
                    {
                    if ((i = Track_LinkSet(t, t2)) > -1)
	                    VisFlag[i] |= 1;
                    return GE_TRUE;
                    }
                }

            if (OneWay)
                {
				if (!Track_LinkExists(t2, t) &&
                    Track_LinkTest(&t->PointList[0], &t2->PointList[0]))
                    {
                    if ((i = Track_LinkSet(t2, t)) > -1)
	                    VisFlag2[i] |= 1;
                    return GE_TRUE;
                    }
                }
            }

        if (s2e < e2e && s2e < s2s && s2e < e2s)
            {
            //s2e closest
            if (OneWay && OneWay2)
                {
				if (!Track_LinkExists(t2, t) &&
                    Track_LinkTest(&t->PointList[0], &t2->PointList[ep2]))
                    {
                    if ((i = Track_LinkSet(t2, t)) > -1)
	                    VisFlag2[i] |= 2;
                    return GE_TRUE;
                    }
                }

            if (OneWay2)
                {
				if (!Track_LinkExists(t2, t) &&
                    Track_LinkTest(&t->PointList[0], &t2->PointList[ep2]))
                    {
                    if ((i = Track_LinkSet(t2, t)) > -1)
	                    VisFlag2[i] |= 2;
                    return GE_TRUE;
                    }
                }

            if (OneWay)
                {
				if (!Track_LinkExists(t2, t) &&
                    Track_LinkTest(&t->PointList[0], &t2->PointList[ep2]))
                    {
                    if ((i = Track_LinkSet(t2, t)) > -1)
	                    VisFlag[i] |= 2;
                    return GE_TRUE;
                    }
                }
            }

        if (e2s < e2e && e2s < s2s && e2s < s2e)
            {
            //e2s closest
            if (OneWay && OneWay2)
                {
				if (!Track_LinkExists(t, t2) &&
                    Track_LinkTest(&t->PointList[ep], &t2->PointList[0]))
                    {
                    if ((i = Track_LinkSet(t, t2)) > -1)
	                    VisFlag[i] |= 4;
                    return GE_TRUE;
                    }
                }
        
            if (OneWay || OneWay2)
                {
				if (!Track_LinkExists(t, t2) &&
                    Track_LinkTest(&t->PointList[ep], &t2->PointList[0]))
                    {
                    if ((i = Track_LinkSet(t, t2)) > -1)
	                    VisFlag[i] |= 4;
                    return GE_TRUE;
                    }
                }
            }
        }
    else
	if (!Track_LinkExists(t, t2) && 
        ((a = Track_LinkTest(&t->PointList[0], &t2->PointList[0])) ||
		(b = Track_LinkTest(&t->PointList[0], &t2->PointList[ep2])) ||
		(c = Track_LinkTest(&t->PointList[ep], &t2->PointList[0])) ||
		(d = Track_LinkTest(&t->PointList[ep], &t2->PointList[ep2]))))
		{
		if ((i = Track_LinkSet(t, t2)) > -1)
			{
			if (a) VisFlag[i] |= 1;
			if (b) VisFlag[i] |= 2;
			if (c) VisFlag[i] |= 4;
			if (d) VisFlag[i] |= 3;
			}
        if ((i = Track_LinkSet(t2, t)) > -1)
			{
			if (a) VisFlag2[i] |= 1;
			if (b) VisFlag2[i] |= 2;
			if (c) VisFlag2[i] |= 4;
			if (d) VisFlag2[i] |= 3;
			}

        return GE_TRUE;
		}

   return GE_FALSE;
   }


//=====================================================================================
//	Track_LinkTracks
//=====================================================================================
void CTrack::Track_LinkTracks()
    {
    Track *t, *t2;

    // look at all tracks finding the closest endpoint
    for (t = &TrackList[0]; t < &TrackList[TrackCount]; t++)
        {
		for (t2 = &TrackList[0]; t2 < &TrackList[TrackCount]; t2++)
			{
            Track_LinkTrack(t, t2);
			}
		}
    }


//=====================================================================================
//	Track_FakeTrack
//=====================================================================================
geBoolean CTrack::Track_FakeTrack(geVec3d *Pos)
    {
    Track *t, *t2;
    TrackPt *tp;
	geBoolean found = GE_FALSE;

	t = &TrackList[MAX_TRACKS];
    tp = t->PointList;

	// set count and position
	t->PointCount = 1;
	tp->Pos = Pos;
	memset(t->Vis, 0, sizeof(t->Vis));

    // look at all tracks finding the closest endpoint

	for (t2 = &TrackList[0]; t2 < &TrackList[TrackCount]; t2++)
		{
        if (Track_LinkTrack(t, t2))
            found = TRUE;
		}

	return (found);
    }


//=====================================================================================
//	Track_FindTrack
//=====================================================================================
Track* CTrack::Track_FindTrack(geVec3d *StartPos, geVec3d *TgtPos, int32 player_dir, int32 *track_type, TrackData *td)
    {
    float near_dist = 999999.0f, ydiff;

    int32 end_point[2] = {0,0};
    int32 near_dir;

    Track *t, *near_track;
    TrackPt *tp, *near_tp;

	TrackPt *stp,*etp;
	float start2tgt_dist, dist0, dist1, bot2start_dist, bot2end_dist, end2tgt_dist, bot2tgt_dist, total_dist2end;
	int32 dir = 1;
	int32 *type;

	geBoolean OneWay = GE_FALSE;
	geBoolean found = GE_FALSE;
	static StartTrack = 0;

	if (td->TrackNdx <= -1)
	{
		StartTrack = 0;
	}
	else
	{
		StartTrack = td->TrackNdx+1;
	}
	
	Track_ClearTrack(td);

    // look at all tracks finding the closest endpoint
    for (t = &TrackList[StartTrack]; t < &TrackList[TrackCount]; t++)
        {
        tp = t->PointList;

        // Skip if is not the track type we are looking for
		if (track_type)
		{
			type = track_type;
	
			while (*type != -1)
			{
				if (t->Type == *type)
					break;
				type++;
			}
			
			if (*type <= -1)
				continue;
		}

		end_point[1] = t->PointCount - 1;

		OneWay = Track_IsOneWay(t);

        ydiff = TRACK_SEARCH_Y_RANGE;

		dist0 = geVec3d_DistanceBetween(StartPos, t->PointList[end_point[0]].Pos);
		dist1 = geVec3d_DistanceBetween(StartPos, t->PointList[end_point[1]].Pos);

		if (t->Type == TRACK_TYPE_TRAVERSE)
		{
			// find the closest point to the dest on the track
			TrackPt *tp;
			float save_start_dist = 999999.0f, 
				  save_end_dist = 999999.0f, dist;

			// find closest points to Start and Tgt Positions
			for (tp = t->PointList; tp < &t->PointList[t->PointCount]; tp++)
			{
				dist = geVec3d_DistanceBetween(tp->Pos, StartPos);

				if (dist < save_start_dist)
				{
					stp = tp;
					save_start_dist = dist;
				}

				dist = geVec3d_DistanceBetween(tp->Pos, TgtPos);

				if (dist < save_end_dist)
				{
					etp = tp;
					save_end_dist = dist;
				}
			}

			if (stp == etp)
				continue;

			// compare pointer locations to see if tgt is past start point
			if (etp > stp)
				dir = 1; // towards end
			else
				dir = -1; // towards start
		}
		else
		if (dist0 <= dist1)
		{
			stp = &t->PointList[end_point[0]];
			etp = &t->PointList[end_point[1]];
			dir = 1;

		}
		else
		{
			if (OneWay)
				continue;

			stp = &t->PointList[end_point[1]];
			etp = &t->PointList[end_point[0]];
			dir = -1;
		}

        if (player_dir == TOWARD_GOAL)
			{
			// early out! - tgtpos is exactly the endpoint
			if (memcmp(TgtPos, etp->Pos, sizeof(*TgtPos)) == 0)
				{
				near_track = t;
				near_tp = stp;
				near_dir = dir;
				found = GE_TRUE;
				break;
				}
			}

        bot2start_dist = geVec3d_DistanceBetween(StartPos, stp->Pos);
		bot2end_dist = geVec3d_DistanceBetween(StartPos, etp->Pos);

        if (bot2start_dist > 1500.0f)
			continue;

        if (bot2start_dist > near_dist)
			continue;

        if (fabs(StartPos->Y - stp->Pos->Y) > ydiff)
            continue;

		// approx total dist to travel - not really used yet but good to have
		total_dist2end = bot2start_dist + Track_Length(t);

        start2tgt_dist = geVec3d_DistanceBetween(stp->Pos, TgtPos);
		end2tgt_dist = geVec3d_DistanceBetween(etp->Pos, TgtPos);

        if (player_dir == TOWARD_GOAL)
		{
			// if the end point is farther than the start point - skip it
			if (end2tgt_dist > start2tgt_dist)
				continue;

            end2tgt_dist = DistWeightedY(etp->Pos, TgtPos, 2.0f);
		    bot2tgt_dist = DistWeightedY(StartPos, TgtPos, 2.0f);

			// if it does not get you closer - skip it
			if (end2tgt_dist > bot2tgt_dist)
				continue;
		}
		else
		if (player_dir == AWAY_FROM_GOAL)
		{
			// if the end point is closer than the start point - skip it
			if (end2tgt_dist < start2tgt_dist)
				continue;

            end2tgt_dist = DistWeightedY(etp->Pos, TgtPos, 2.0f);
		    bot2tgt_dist = DistWeightedY(StartPos, TgtPos, 2.0f);

			// if it does not get you further then skip it
			if (end2tgt_dist < bot2tgt_dist)
				continue;
		}

        if (!CanSeePointToPoint(StartPos, stp->Pos))
			continue;

		found = GE_TRUE;
        near_dist = bot2start_dist;
        near_track = t;
        near_tp = stp;
		near_dir = dir;
	}

    if (found)
        {
        // if can see the point, return the track number
        td->TrackNdx = near_track - &TrackList[0];
        td->PointNdx = near_tp - &TrackList[td->TrackNdx].PointList[0];
        td->TrackDir = near_dir;
        return (near_track);
        }
    else
        {
        Track_ClearTrack(td);
        return (NULL);
        }
    }

//=====================================================================================
//	Track_FindFarTrack
//=====================================================================================
Track* CTrack::Track_FindFarTrack(geVec3d *StartPos)
    {
		float far_dist = 0.0f;
		
		int32 end_point[2] = {0,0};
		
		Track *t, *far_track;
		
		TrackPt *stp;
		float bot2start_dist;
		int32 dir = 1;
		
		geBoolean OneWay = GE_FALSE;
		geBoolean found = GE_FALSE;
		
		int32 StartTrack = 0;

		// look at all tracks finding the closest endpoint
		for (t = &TrackList[StartTrack]; t < &TrackList[TrackCount]; t++)
        {
			stp = t->PointList;
			//end_point[1] = t->PointCount - 1;
			
			OneWay = Track_IsOneWay(t);
			
			bot2start_dist = geVec3d_DistanceBetween(StartPos, stp->Pos);
			
			if (bot2start_dist < far_dist)
				continue;
			
			if (CanSeePointToPoint(StartPos, stp->Pos))
                continue;
			
			found = GE_TRUE;
			far_dist = bot2start_dist;
			far_track = t;
		}
		
		if (found)
            {
			return (far_track);
            }
		
		return (NULL);
    }


//=====================================================================================
//	Track_FindCloseTrack
//=====================================================================================
Track* CTrack::Track_FindCloseTrack(geVec3d *StartPos, float ydiff)
    {
		float near_dist = 999999.0f;
		
		int32 end_point[2] = {0,0};
		
		Track *t, *near_track;
		TrackPt *tp;
		
		TrackPt *stp;
		float bot2start_dist;
		int32 dir = 1;
		
		geBoolean OneWay = GE_FALSE;
		geBoolean found = GE_FALSE;
		
		int32 StartTrack = 0;

		// look at all tracks finding the closest endpoint
		for (t = &TrackList[StartTrack]; t < &TrackList[TrackCount]; t++)
        {
			tp = t->PointList;
			end_point[1] = t->PointCount - 1;
			
			OneWay = Track_IsOneWay(t);
			
			ydiff = TRACK_SEARCH_Y_RANGE;
			
			{
			// find the closest point to the dest on the track
			TrackPt *tp;
			float save_start_dist = 999999.0f, 
				  save_end_dist = 999999.0f, dist;

			// find closest points to Start and Tgt Positions
			for (tp = t->PointList; tp < &t->PointList[t->PointCount]; tp++)
			{
				dist = geVec3d_DistanceBetween(tp->Pos, StartPos);

				if (dist < save_start_dist)
				{
					stp = tp;
					save_start_dist = dist;
				}
			}
			}
			
			bot2start_dist = geVec3d_DistanceBetween(StartPos, stp->Pos);
			
			if (bot2start_dist > 1500.0f)
				continue;
			
			if (bot2start_dist > near_dist)
				continue;
			
			if (fabs(StartPos->Y - stp->Pos->Y) > ydiff)
				continue;

			if (!CanSeePointToPoint(StartPos, stp->Pos))
				{
				TrackPt *tp;
				// see if you can see ANY points on this track
				for (tp = t->PointList; tp < &t->PointList[t->PointCount]; tp++)
				{
					if (stp == tp)
						continue;

					if (CanSeePointToPoint(StartPos, tp->Pos))
						goto found;
				}

				continue;
				}
			
			found:

			found = GE_TRUE;
			near_dist = bot2start_dist;
			near_track = t;
		}
		
		if (found)
        {
			return (near_track);
        }
		
		return (NULL);
    }

//=====================================================================================
//	Track_FindMultiTrack
//=====================================================================================
geBoolean CTrack::Track_FindMultiTrack(geVec3d *StartPos, geVec3d *EndPos, int32 dir, TrackCB *cb, Stack *nodestack)
	{
	int32 endnode, startnode;
	int32 s_plc, s_end, i, j, k, nodenum;
	Track *t, *n;
	geBoolean found = GE_FALSE;
    	
	if (!TrackCount)
		return GE_FALSE;

	//start at the end for ease of traversing
	if (!Track_FakeTrack(StartPos))
		return GE_FALSE;

	t = &TrackList[MAX_TRACKS];
	assert(t);
	startnode = t - TrackList;

	// early out
	if (!t->Vis[0])
		return GE_FALSE;

    if (dir == -1)
	    t = Track_FindFarTrack(EndPos);
    else
    if (dir == 1)
	    t = Track_FindCloseTrack(EndPos, TRACK_SEARCH_Y_RANGE*1.5f);

	if (!t)
		return GE_FALSE;

	endnode = t - TrackList;
	
	//clear bitsearch array
	memset(bitarr, 0, sizeof(bitarr));
	
	// Create a node
	searchnode[0] = startnode;
	searchparent[0] = -1;
	
	// startnode to searched
	SET_BITARR(bitarr,startnode);
	
	// search place and search end
	for(s_plc = 0, s_end = 1; s_plc < s_end; s_plc++)
	{
		//loop through all nodes "visible" from this node
		n = &TrackList[searchnode[s_plc]];
		
		for (i = 0; n->Vis[i]; i++)
		{
			nodenum = n->Vis[i] - &TrackList[0];
			
			// look at ALL nextsectors !!!!!! s_plc is parent
			if (nodenum < 0) 
				continue;
			
			// only look at sector if its not been looked at before
			if (!TEST_BITARR(bitarr,nodenum))
			{
				// mark sector as searched
				SET_BITARR(bitarr,nodenum);

				if (!cb->CB(cb->Data, &TrackList[nodenum]))
					continue;

				// save off this sector at the END!!!!!
				searchnode[s_end] = nodenum;
				// save off the parent sector at the END!!!!!
				searchparent[s_end] = s_plc;
				// increment s_end (search end)
				s_end++;
				
				if (nodenum == endnode)
				{
					// FOUND -A- PATH!
					found = GE_TRUE;

                    // clear stack
                    StackReset(nodestack);

                    for(k = s_end-1, j=0; k >= 0; k = searchparent[k])
    					{
						if (searchnode[k] >= MAX_TRACKS)
							continue;

                        StackPush(nodestack, searchnode[k]);
                        }

					goto finish;
				}
			}
		}
	}

finish:

	// clean links to fake track
	for (t = &TrackList[0]; t < &TrackList[TrackCount]; t++)
    {
		for (i = 0; t->Vis[i]; i++)
			{
			if (t->Vis[i] >= &TrackList[MAX_TRACKS])
				t->Vis[i] = 0;
			}
	}

	return (found);

	}


//=====================================================================================
//	Track_NextMultiTrack
//=====================================================================================
geBoolean CTrack::Track_NextMultiTrack(geVec3d *StartPos, int32 TrackNdx, TrackData *td)
    {
	Track *t;
	TrackPt *tp, *ep;

	geBoolean OneWay;

	int32 tpoint,dir;
	float dist2end,dist2start;

	assert(TrackNdx > -1);

	// look at all tracks finding the closest endpoint
	t = &TrackList[TrackNdx];
	tp = t->PointList;
	ep = &t->PointList[t->PointCount - 1];
	
	OneWay = Track_IsOneWay(t);

	if (OneWay)
		{
		td->TrackNdx = t - &TrackList[0];
		td->PointNdx = 0;
		td->TrackDir = 1;
		return GE_TRUE;
		}

	dist2start = geVec3d_DistanceBetween(StartPos, tp->Pos);
	dist2end = geVec3d_DistanceBetween(StartPos, ep->Pos);

	if (dist2start < dist2end)
	{
		// try and see the closest point
		if (CanSeePointToPoint(StartPos, tp->Pos))
			{
			dir = 1;
			tpoint = 0;
			}
		else
			{
			dir = -1;
			tpoint = t->PointCount - 1;
			}
	}
	else
	{
		// try and see the closest point
		if (CanSeePointToPoint(StartPos, ep->Pos))
			{
			dir = -1;
			tpoint = t->PointCount - 1;
			}
		else
			{
			dir = 1;
			tpoint = 0;
			}
	}

	td->TrackNdx = t - &TrackList[0];
	td->PointNdx = tpoint;
	td->TrackDir = dir;

	return GE_TRUE;
}




