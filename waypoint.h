#ifndef __BOTRIX_WAYPOINT_H__
#define __BOTRIX_WAYPOINT_H__


#include <good/bitset.h>
#include <good/graph.h>

#include "mod.h"
#include "source_engine.h"
#include "types.h"

#include "vector.h"


//****************************************************************************************************************
/// Class that represents waypoint on a map.
//****************************************************************************************************************
class CWaypoint
{

public: // Methods.
    /// Return true if waypoint id can be valid. Use CWaypoints::IsValid() to actualy verify waypoint range.
    static inline bool IsValid(TWaypointId id) { return (id >= 0); }

public: // Methods.
    /// Default constructor.
    CWaypoint(): vOrigin(), iFlags(0), iArgument(0), iAreaId(0) {}

    /// Constructor with parameters.
    CWaypoint( const Vector& vOrigin, int iFlags = FWaypointNone, int iArgument = 0, TAreaId iAreaId = 0 ):
        vOrigin(vOrigin), iFlags(iFlags), iArgument(iArgument), iAreaId(iAreaId) {}

    /// Get waypoint flags for needed entity type (health, armor, weapon, ammo).
    static TWaypointFlags GetFlagsFor( TItemType iEntityType ) { return m_aFlagsForEntityType[iEntityType]; }

    /// Get first angle from waypoint argument.
    static void GetFirstAngle( QAngle& a, int iArgument )
    {
        int iPitch = (iArgument << 16) >> (16+9);   // 7 higher bits of low word (-64..63).
        int iYaw = (iArgument << (16+7)) >> (16+7); // 9 lower bits of low word (-256..255 but used -180..+180).
        a.x = iPitch; a.y = iYaw; a.z = 0;
    }

    /// Get first angle from waypoint argument.
    static void SetFirstAngle( int iPitch, int iYaw, int& iArgument )
    {
        //BASSERT( -64 <= iPitch && iPitch <= 63 && -128 <= iYaw && iYaw <= 128 );
        SET_1ST_WORD( ((iPitch & 0x7F) << 9) | (iYaw &0x1FF), iArgument );
    }

    /// Get second angle from waypoint argument.
    static void GetSecondAngle( QAngle& a, int iArgument )
    {
        int iPitch = iArgument >> (16+9);   // 7 higher bits of high word (-64..63).
        int iYaw = (iArgument << 7) >> (16+7); // 9 lower bits of low word (-256..255 but used -180..+180).
        a.x = iPitch; a.y = iYaw; a.z = 0;
    }

    /// Get second angle from waypoint argument.
    static void SetSecondAngle( int iPitch, int iYaw, int& iArgument )
    {
        //BASSERT( -64 <= iPitch && iPitch <= 63 && -128 <= iYaw && iYaw <= 128 );
        SET_2ND_WORD( ((iPitch & 0x7F) << 9) | (iYaw &0x1FF), iArgument );
    }


    /// Get ammo from waypoint argument.
    static int GetAmmo( bool& bIsSecondary, int iArgument )
    {
        int iResult = GET_1ST_BYTE(iArgument);
        bIsSecondary = (iResult & 0x80) != 0;
        return iResult & 0x7F;
    }

    /// Set ammo for waypoint argument.
    static void SetAmmo( int iAmmo, bool bIsSecondary, int& iArgument ) { SET_1ST_BYTE(iAmmo | (bIsSecondary ? 0x80 : 0), iArgument); }


    /// Get weapon index from waypoint argument.
    static int GetWeaponIndex( int iArgument ) { return GET_2ND_BYTE(iArgument) & 0x0F; }

    /// Get weapon subindex from waypoint argument.
    static int GetWeaponSubIndex( int iArgument ) { return GET_2ND_BYTE(iArgument) >> 4; }

    /// Set weapon index/subindex for waypoint argument.
    static void SetWeapon( int iIndex, int iSubIndex, int& iArgument ) { SET_2ND_BYTE( iIndex | (iSubIndex<<4), iArgument ); }


    /// Get armor from waypoint argument.
    static int GetArmor( int iArgument ) { return GET_3RD_BYTE(iArgument); }

    /// Set armor for waypoint argument.
    static void SetArmor( int iArmor, int& iArgument ) { SET_3RD_BYTE(iArmor, iArgument); }


    /// Get health from waypoint argument.
    static int GetHealth( int iArgument ) { return GET_4TH_BYTE(iArgument); }

    /// Set health for waypoint argument.
    static void SetHealth( int iHealth, int& iArgument ) { SET_4TH_BYTE(iHealth, iArgument); }


    /// Get button index from waypoint argument.
    static TItemIndex GetButton( int iArgument ) { return GET_3RD_BYTE(iArgument); }

    /// Set button index for waypoint argument.
    static void SetButton( TItemIndex iButton, int& iArgument ) { SET_3RD_BYTE(iButton, iArgument); }


    /// Return true if point v 'touches' this waypoint.
    inline bool IsTouching( const Vector& v, bool bOnLadder ) const
    {
        return CUtil::IsPointTouch3d(vOrigin, v, bOnLadder ? CMod::iPointTouchLadderSquaredZ : CMod::iPointTouchSquaredZ, CMod::iPointTouchSquaredXY);
    }

    /// Get color of waypoint.
    void GetColor( unsigned char& r, unsigned char& g, unsigned char& b ) const;

    /// Draw this waypoint for given amount of time.
    void Draw( TWaypointId iWaypointId, TWaypointDrawFlags iDrawType, float fDrawTime ) const;


public: // Members and constants.
    static const int MIN_ANGLE_PITCH = -64;  ///< Minimum pitch that can be used in angle argument.
    static const int MAX_ANGLE_PITCH = +63;  ///< Maximun pitch that can be used in angle argument.
    static const int MIN_ANGLE_YAW = -180;   ///< Minimum yaw that can be used in angle argument.
    static const int MAX_ANGLE_YAW = +180;   ///< Maximun yaw that can be used in angle argument.

    static const int DRAW_INTERVAL = 1;      ///< Waypoint interval for drawing in seconds.
    static const int WIDTH = 8;              ///< Waypoint width for drawing.
    static const int PATH_WIDTH = 4;         ///< Waypoint's path width for drawing.

    static const int MAX_RANGE = 256;        ///< Max waypoint range to invalidate current waypoint.

    static int iWaypointTexture;             ///< Texture of waypoint. Precached at CWaypoints::Load().
    static int iDefaultDistance;             ///< Max waypoint distance to automatically add path to nearby waypoints.

    Vector vOrigin;                          ///< Coordinates of waypoint (x, y, z).
    TWaypointFlags iFlags;                   ///< Waypoint flags.
    int iArgument;                           ///< Waypoint argument.
    TAreaId iAreaId;                         ///< Area id where waypoint belongs to (like "Bombsite A" / "Base CT" in counter-strike).

    //unsigned char iPlayersCount;             ///< Count of players that reached this waypoint.

protected:
    static const TWaypointFlags m_aFlagsForEntityType[EItemTypeNotObject];
};


//****************************************************************************************************************
/// Class that represents path between 2 adjacent waypoints.
//****************************************************************************************************************
class CWaypointPath
{
public:
    CWaypointPath( float fLength, TPathFlags iFlags = FPathNone, unsigned short iArgument = 0 ):
        fLength(fLength), iFlags(iFlags), iArgument(iArgument) {}

    bool IsActionPath() { return iFlags != 0; }

    bool HasDemo() { return FLAG_SOME_SET(FPathDemo, iFlags); }
    int DemoNumber() { return iFlags & (FPathDemo-1); }

    float fLength;                           ///< Path length.
    TPathFlags iFlags;                       ///< Path flags.

    /// Path argument. At 1rst byte there is time to wait before action (in deciseconds). 2nd byte is action duration.
    unsigned short iArgument;
};


class CClient; // Forward declaration.


//****************************************************************************************************************
/// Class that represents set of waypoints on a map.
//****************************************************************************************************************
class CWaypoints
{

public: // Types and constants.
    /// Graph that represents graph of waypoints.
    typedef good::graph< CWaypoint, CWaypointPath, good::vector, good::vector > WaypointGraph;
    typedef WaypointGraph::node_t WaypointNode;     ///< Node of waypoint graph.
    typedef WaypointGraph::arc_t WaypointArc;       ///< Graph arc, i.e. path between two waypoints.
    typedef WaypointGraph::node_it WaypointNodeIt;  ///< Node iterator.
    typedef WaypointGraph::arc_it WaypointArcIt;    ///< Arc iterator.
    //typedef WaypointGraph::node_id TWaypointId;     ///< Type for node identifier.

    static bool bValidVisibilityTable;              ///< When waypoints are modified vis-table needs to be recalculated.
    static float fNextDrawWaypointsTime;            ///< Next draw time of waypoints (draw once per second).

public: // Methods.
    /// Return true if waypoint id is valid. Verifies that waypoint is actually exists.
    static bool IsValid( TWaypointId id ) { return m_cGraph.is_valid(id); }

    /// Return waypoints count.
    static int Size() { return (int)m_cGraph.size(); }

    /// Clear waypoints.
    static void Clear()
    {
        ClearLocations();
        m_cGraph.clear();
        m_cAreas.clear();
        m_aVisTable.clear();
        bValidVisibilityTable = false;
    }

    /// Save waypoints to a file.
    static bool Save();

    /// Load waypoints from file (but you must clear them first).
    static bool Load();


    /// Get waypoint.
    static inline CWaypoint& Get( TWaypointId id ) { return m_cGraph[id].vertex; }

    /// Check if waypoint @p iTo is visible from @p iFrom.
    static inline bool IsVisible( TWaypointId iFrom, TWaypointId iTo )
    {
        return bValidVisibilityTable ? m_aVisTable[iFrom].test(iTo) : false;
    }

    /// Get random neighbour which is visible to given waypoint.
    static TWaypointId GetRandomNeighbour( TWaypointId iWaypoint, TWaypointId iTo, bool bVisible );

    /// Get nearest neighbour to given waypoint.
    static TWaypointId GetNearestNeighbour( TWaypointId iWaypoint, TWaypointId iTo, bool bVisible );

    /// Get farest neighbour to given waypoint.
    static TWaypointId GetFarestNeighbour( TWaypointId iWaypoint, TWaypointId iTo, bool bVisible );

    /// Return true if there is a path from waypoint source to waypoint dest.
    static inline bool HasPath( TWaypointId source, TWaypointId dest )
    {
        WaypointNode& w = m_cGraph[source];
        return w.find_arc_to(dest) != w.neighbours.end();
    }

    /// Get waypoint path.
    static CWaypointPath* GetPath( TWaypointId iFrom, TWaypointId iTo );

    /// Get waypoint node (waypoint + neighbours).
    static WaypointNode& GetNode( TWaypointId id ) { return m_cGraph[id]; }

    /// Add waypoint.
    static TWaypointId Add( const Vector& vOrigin, TWaypointFlags iFlags = FWaypointNone, int iArgument = 0, int iAreaId = 0 );

    /// Remove waypoint.
    static void Remove( TWaypointId id );

    /// Move waypoint to new position.
    static inline void Move( TWaypointId id, const Vector& vOrigin )
    {
        GoodAssert( CWaypoint::IsValid(id) );
        CWaypoint& cWaypoint = Get(id);
        RemoveLocation( id, cWaypoint.vOrigin );
        AddLocation(id, vOrigin);
        cWaypoint.vOrigin = vOrigin;
    }

    /// Add path from waypoint iFrom to waypoint iTo.
    static bool AddPath( TWaypointId iFrom, TWaypointId iTo, float fDistance = 0.0f, TPathFlags iFlags = FPathNone );

    /// Remove path from waypoint iFrom to waypoint iTo.
    static bool RemovePath( TWaypointId iFrom, TWaypointId iTo );

    /// Create waypoint paths if reachable, from iFrom to iTo and viceversa.
    static void CreatePathsWithAutoFlags( TWaypointId iWaypoint1, TWaypointId iWaypoint2, bool bIsCrouched );

    /// Create waypoint paths to nearests waypoints.
    static void CreateAutoPaths( TWaypointId id, bool bIsCrouched );


    /// Get nearest waypoint to given position.
    static TWaypointId GetNearestWaypoint( const Vector& vOrigin, const good::bitset* aOmit = NULL, bool bNeedVisible = true,
                                           float fMaxDistance = CWaypoint::MAX_RANGE, TWaypointFlags iFlags = FWaypointNone );

    /// Get any waypoint with some of the given flags set.
    static TWaypointId GetAnyWaypoint( TWaypointFlags iFlags = FWaypointNone );


    /// Get areas names for current map.
    static StringVector& GetAreas() { return m_cAreas; }

    /// Get area id from name.
    static TAreaId GetAreaId( const good::string& sName )
    {
        StringVector::const_iterator it( good::find(m_cAreas.begin(), m_cAreas.end(), sName) );
        return ( it == m_cAreas.end() )  ?  EAreaIdInvalid  :  ( it - m_cAreas.begin() );
    }

    /// Add new area name.
    static TAreaId AddAreaName( const good::string& sName ) { m_cAreas.push_back(sName); return m_cAreas.size()-1; }

    /// Get waypoint at which player is looking at.
    static TWaypointId GetAimedWaypoint( const Vector& vOrigin, const QAngle& ang );

    /// Draw nearest waypoints around player.
    static void Draw( CClient* pClient );


protected:
    friend class CWaypointNavigator; // Get access to m_cGraph (for A* search implementation).

    // Get path color.
    static void GetPathColor( TPathFlags iFlags, unsigned char& r, unsigned char& g, unsigned char& b );

    // Draw waypoint paths.
    static void DrawWaypointPaths( TWaypointId id, TPathDrawFlags iPathDrawFlags );

    // Draw visibles waypoints.
    static void DrawVisiblePaths( TWaypointId id, TPathDrawFlags iPathDrawFlags );

    // Buckets are 3D areas that we will use to optimize nearest waypoints finding.
    static const int BUCKETS_SIZE_X = 96;
    static const int BUCKETS_SIZE_Y = 96;
    static const int BUCKETS_SIZE_Z = 96;

    static const int BUCKETS_SPACE_X = CUtil::iMaxMapSize/BUCKETS_SIZE_X;
    static const int BUCKETS_SPACE_Y = CUtil::iMaxMapSize/BUCKETS_SIZE_Y;
    static const int BUCKETS_SPACE_Z = CUtil::iMaxMapSize/BUCKETS_SIZE_Z;

    // Get bucket indexes in array of buckets.
    static int GetBucketX( float fPositionX ) { return (int)(fPositionX + CUtil::iHalfMaxMapSize) / BUCKETS_SPACE_X; }
    static int GetBucketY( float fPositionY ) { return (int)(fPositionY + CUtil::iHalfMaxMapSize) / BUCKETS_SPACE_Y; }
    static int GetBucketZ( float fPositionZ ) { return (int)(fPositionZ + CUtil::iHalfMaxMapSize) / BUCKETS_SPACE_Z; }

    // Get adjacent buckets.
    static void GetBuckets( int x, int y, int z, int& minX, int& minY, int& minZ, int& maxX, int& maxY, int& maxZ )
    {
        minX = (x>0) ? x-1 : x;
        minY = (y>0) ? y-1 : y;
        minZ = (z>0) ? z-1 : z;

        maxX = (x == BUCKETS_SIZE_X-1) ? BUCKETS_SIZE_X-1 : x+1;
        maxY = (y == BUCKETS_SIZE_Y-1) ? BUCKETS_SIZE_Y-1 : y+1;
        maxZ = (z == BUCKETS_SIZE_Z-1) ? BUCKETS_SIZE_Z-1 : z+1;
    }

    // Add location for waypoint.
    static inline void AddLocation( TWaypointId id, const Vector& vOrigin )
    {
        m_cBuckets[GetBucketX(vOrigin.x)][GetBucketY(vOrigin.y)][GetBucketZ(vOrigin.z)].push_back(id);
    }

    // Add location for waypoint.
    static inline void RemoveLocation( TWaypointId id, const Vector& vOrigin )
    {
        Bucket& cBucket = m_cBuckets[GetBucketX(vOrigin.x)][GetBucketY(vOrigin.y)][GetBucketZ(vOrigin.z)];
        cBucket.erase( find(cBucket, id) );
    }

    // Add location for waypoint.
    static void DecrementLocationIds( TWaypointId id );

    // Clear all locations.
    static void ClearLocations()
    {
        if ( m_cGraph.size() > 0 )
        {
            for (int x=0; x<BUCKETS_SIZE_X; ++x)
                for (int y=0; y<BUCKETS_SIZE_Y; ++y)
                    for (int z=0; z<BUCKETS_SIZE_Z; ++z)
                        m_cBuckets[x][y][z].clear();
        }
    }

    typedef good::vector<TWaypointId> Bucket;
    static Bucket m_cBuckets[BUCKETS_SIZE_X][BUCKETS_SIZE_Y][BUCKETS_SIZE_Z]; // 3D hash table of arrays of waypoint IDs.

    static StringVector m_cAreas;      // Areas names.

    static WaypointGraph m_cGraph;         // Waypoints graph.

    static good::vector< good::bitset > m_aVisTable;
};


#endif // __BOTRIX_WAYPOINT_H__
