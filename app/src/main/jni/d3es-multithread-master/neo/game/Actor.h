/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#ifndef __GAME_ACTOR_H__
#define __GAME_ACTOR_H__

#include "AFEntity.h"
#include "IK.h"
#include "PlayerView.h"

/*
===============================================================================

	idActor

===============================================================================
*/

extern const idEventDef AI_EnableEyeFocus;
extern const idEventDef AI_DisableEyeFocus;
extern const idEventDef EV_Footstep;
extern const idEventDef EV_FootstepLeft;
extern const idEventDef EV_FootstepRight;
extern const idEventDef EV_EnableWalkIK;
extern const idEventDef EV_DisableWalkIK;
extern const idEventDef EV_EnableLegIK;
extern const idEventDef EV_DisableLegIK;
extern const idEventDef AI_SetAnimPrefix;
extern const idEventDef AI_PlayAnim;
extern const idEventDef AI_PlayCycle;
extern const idEventDef AI_AnimDone;
extern const idEventDef AI_SetBlendFrames;
extern const idEventDef AI_GetBlendFrames;
extern const idEventDef AI_SetState;
extern const idEventDef EV_FootprintLeft;	// HUMANHEAD JRM
extern const idEventDef EV_FootprintRight;	// HUMANHEAD JRM
extern const idEventDef AI_DisablePain;		// HUMANHEAD JRM
extern const idEventDef AI_EnablePain;		// HUMANHEAD JRM

class idDeclParticle;
class hhVehicle;		// HUMANHEAD pdm
class hhBindController;	// HUMANHEAD pdm

class idAnimState {
public:
	bool					idleAnim;
	idStr					state;
	int						animBlendFrames;
	int						lastAnimBlendFrames;		// allows override anims to blend based on the last transition time

public:
							idAnimState();
							~idAnimState();

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	void					Init( idActor *owner, idAnimator *_animator, int animchannel );
	void					Shutdown( void );
	void					SetState( const char *name, int blendFrames );
	void					StopAnim( int frames );
	void					PlayAnim( int anim );
	void					CycleAnim( int anim );
	void					BecomeIdle( void );
	bool					UpdateState( void );
	bool					Disabled( void ) const;
	void					Enable( int blendFrames );
	void					Disable( void );
	bool					AnimDone( int blendFrames ) const;
	bool					IsIdle( void ) const;
	animFlags_t				GetAnimFlags( void ) const;

	bool	   		 		InGravityZone(void); // HUMANHEAD mdl:  Returns true if the player is inside an hhGravityZoneBase entity
#ifdef HUMANHEAD //jsh
	void					PlayAnimSkip( int anim, int delay );
#endif

private:
	idActor *				self;
	idAnimator *			animator;
	idThread *				thread;
	int						channel;
	bool					disabled;
};

class idAttachInfo {
public:
	idEntityPtr<idEntity>	ent;
	int						channel;
};

typedef struct {
	jointModTransform_t		mod;
	jointHandle_t			from;
	jointHandle_t			to;
} copyJoints_t;

class idActor : public idAFEntity_Gibbable {
public:
	CLASS_PROTOTYPE( idActor );

	int						team;
	int						rank;				// monsters don't fight back if the attacker's rank is higher
	idMat3					viewAxis;			// view axis of the actor

	idLinkList<idActor>		enemyNode;			// node linked into an entity's enemy list for quick lookups of who is attacking him
	idLinkList<idActor>		enemyList;			// list of characters that have targeted the player as their enemy

	// HUMANHEAD nla
	idScriptBool			AI_BOUND;			// HUMANHEAD pdm
	idScriptBool			AI_VEHICLE;			// HUMANHEAD pdm
	// HUMANHEAD END
public:
							idActor( void );
	virtual					~idActor( void );

	void					Spawn( void );
	virtual void			Restart( void );

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	//HUMANHEAD rww - general network handling of all actors
	virtual void			WriteToSnapshot( idBitMsgDelta &msg ) const;
	virtual void			ReadFromSnapshot( const idBitMsgDelta &msg );
	virtual void			ClientPredictionThink( void );
	virtual void			SetSkin( const idDeclSkin *skin );
	//HUMANHEAD END

	virtual void			Hide( void );
	virtual void			Show( void );
	virtual int				GetDefaultSurfaceType( void ) const;
	virtual void			ProjectOverlay( const idVec3 &origin, const idVec3 &dir, float size, const char *material );

	virtual bool			LoadAF( void );
	void					SetupBody( void );

	void					CheckBlink( void );

	virtual bool			GetPhysicsToVisualTransform( idVec3 &origin, idMat3 &axis );
	virtual bool			GetPhysicsToSoundTransform( idVec3 &origin, idMat3 &axis );

							// script state management
	void					ShutdownThreads( void );
	virtual bool			ShouldConstructScriptObjectAtSpawn( void ) const;
	virtual idThread *		ConstructScriptObject( void );
	void					UpdateScript( void );
	const function_t		*GetScriptFunction( const char *funcname );
	void					SetState( const function_t *newState );
	void					SetState( const char *statename );

							// vision testing
	virtual // HUMANHEAD aob: made virtual
	void					SetEyeHeight( float height );
	virtual // HUMANHEAD aob: made virtual
	float					EyeHeight( void ) const;
	virtual // HUMANHEAD aob: made virtual
	idVec3					EyeOffset( void ) const;
	virtual // HUMANHEAD aob: made virtual
	idVec3					GetEyePosition( void ) const;
	virtual void			GetViewPos( idVec3 &origin, idMat3 &axis ) const;
	void					SetFOV( float fov );
	virtual // HUMANHEAD jrm - made virtual
	bool					CheckFOV( const idVec3 &pos ) const;
	// HUMANHEAD JRM - made virtual
	virtual
#ifdef HUMANHEAD
	bool					CanSee( idEntity *ent, bool useFOV );
#else
	bool					CanSee( idEntity *ent, bool useFOV ) const;
#endif
	bool					PointVisible( const idVec3 &point ) const;
	virtual void			GetAIAimTargets( const idVec3 &lastSightPos, idVec3 &headPos, idVec3 &chestPos );


	// HUMANHEAD
	void					SetAnimPrefix( const char *prefix );
	const char *			GetAnimPrefix( void );
	idMat3					GetGravViewAxis(void)	const;	// HUMANHEAD JRM - returns clipmodel axis * viewAxis
	virtual float			EyeHeightIdeal( void ) const;
	virtual bool			ShouldRemainAlignedToAxial() const;
	virtual bool			IsWallWalking() const { return(false); }
	virtual bool			Give(const char *statname, const char *value);
	virtual void			Possess( idEntity *possessor ); // HUMANHEAD aob
	virtual void			Unpossess(); // HUMANHEAD aob
	virtual bool			CanBePossessed( void ); // HUMANHEAD cjr

	virtual void			UpdateOrientation( const idAngles& _untransformedViewAngles ) {}
	virtual void			DetermineOwnerPosition( idVec3 &ownerOrigin, idMat3 &ownerAxis );//aob
	virtual idVec3			ApplyLandDeflect( const idVec3& pos, float scale ) { return pos; }
	virtual const idMat3&	GetAxis( int id = 0 ) const { return viewAxis; }
	virtual void			PlayFootstepSound();
	virtual void			PlayFootstepSoundMatter( const trace_t& trace );
	virtual void			Event_AnimSyncLegs();			// nla
	virtual idVec3 			GetAimPosition() const;

	virtual void			EnterVehicle( hhVehicle* vehicle );
	virtual void			ExitVehicle( hhVehicle* vehicle );
	virtual void			GetPilotInput( usercmd_t& pilotCmds, idAngles& pilotViewAngles ) {}
	virtual void			ResetClipModel();
	class hhPilotVehicleInterface*	GetVehicleInterface();
	const hhPilotVehicleInterface*	GetVehicleInterface() const;
	void					SetVehicleInterface( hhPilotVehicleInterface* newVehicleInterface );
	bool					InVehicle() const;
	void					DisableIK()							{	walkIK.DisableAll();	}
	void					EnableIK()							{	walkIK.EnableAll();		}
	virtual void			BecameBound(hhBindController *b)	{}
	virtual void			BecameUnbound(hhBindController *b)	{}
	bool					PlayCrashLandSound( const trace_t& trace, const float volumeScale );
	idVec3					DetermineDeltaCollisionVelocity( const idVec3& currentVel, const trace_t& trace );
	virtual void			PlayPainSound();
	idAnimState*			GetLegsAnim(void)	{return &legsAnim;}
	idAnimState*			GetTorsoAnim(void)	{return &torsoAnim;}
	idAnimState*			GetHeadAnim(void)	{return &headAnim;}
	virtual void			SetAxis( const idMat3& axis ) { viewAxis = axis; UpdateVisuals(); }
	virtual void			SetShaderParm( int parmnum, float value );
	idEntity *				GetHead();
	// HUMANHEAD END

							// damage
	void					SetupDamageGroups( void );
	virtual	void			Damage( idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location );
	int						GetDamageForLocation( int damage, int location, bool headMultiplier = false );
	const char *			GetDamageGroup( int location );
	void					ClearPain( void );
	virtual bool			Pain( idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location );

							// model/combat model/ragdoll
	void					SetCombatModel( void );
	idClipModel *			GetCombatModel( void ) const;
	virtual void			LinkCombat( void );
	virtual void			UnlinkCombat( void );
	bool					StartRagdoll( void );
	void					StopRagdoll( void );
	virtual bool			UpdateAnimationControllers( void );

							// delta view angles to allow movers to rotate the view of the actor
	const idAngles &		GetDeltaViewAngles( void ) const;
	void					SetDeltaViewAngles( const idAngles &delta );

	bool					HasEnemies( void ) const;
	idActor *				ClosestEnemyToPoint( const idVec3 &pos );
	idActor *				EnemyWithMostHealth();

	virtual bool			OnLadder( void ) const;

	virtual void			GetAASLocation( idAAS *aas, idVec3 &pos, int &areaNum ) const;

	void					Attach( idEntity *ent );

	virtual void			Teleport( const idVec3 &origin, const idAngles &angles, idEntity *destination );

	virtual	renderView_t *	GetRenderView();

							// animation state control
	int						PlayAnim( int channel, const char* name );
	int						GetAnim( int channel, const char *name );
	void					UpdateAnimState( void );
	void					SetAnimState( int channel, const char *name, int blendFrames );
	const char *			GetAnimState( int channel ) const;
	bool					InAnimState( int channel, const char *name ) const;
	const char *			WaitState( void ) const;
	void					SetWaitState( const char *_waitstate );
	bool					AnimDone( int channel, int blendFrames ) const;
	virtual void			SpawnGibs( const idVec3 &dir, const char *damageDefName );

	bool					InGravityZone( void ); // HUMANHEAD mdl
	virtual void			Portalled(idEntity *portal); // HUMANHEAD mdl

	idEntity*				GetHeadEntity()
	{
		return head.GetEntity();
	};

	void					PlayFootStepSound( void );

protected:
	friend class			idAnimState;

	// Carl: navigation (originally in AI.h)
	idAAS* 					aas;
	int						travelFlags;

	float					fovDot;				// cos( fovDegrees )
	idVec3					eyeOffset;			// offset of eye relative to physics origin
	idVec3					modelOffset;		// offset of visual model relative to the physics origin

	idAngles				deltaViewAngles;	// delta angles relative to view input angles

	int						pain_debounce_time;	// next time the actor can show pain
	int						pain_delay;			// time between playing pain sound
	int						pain_threshold;		// how much damage monster can take at any one time before playing pain animation

	idStrList				damageGroups;		// body damage groups
	idList<float>			damageScale;		// damage scale per damage gruop

	bool						use_combat_bbox;	// whether to use the bounding box for combat collision
	idEntityPtr<idAFAttachment>	head;
	idList<copyJoints_t>		copyJoints;			// copied from the body animation to the head model

	// state variables
	const function_t		*state;
	const function_t		*idealState;

	// joint handles
	jointHandle_t			leftEyeJoint;
	jointHandle_t			rightEyeJoint;
	jointHandle_t			soundJoint;

	idIK_Walk				walkIK;
    // Koz
    idIK_Reach				armIK;
    // Koz

	idStr					animPrefix;
	idStr					painAnim;

	// blinking
	int						blink_anim;
	int						blink_time;
	int						blink_min;
	int						blink_max;

	//HUMANHEAD: aob
	hhPilotVehicleInterface*	vehicleInterface;
	//HUMANHEAD END

	// script variables
	idThread *				scriptThread;
	idStr					waitState;
	idAnimState				headAnim;
	idAnimState				torsoAnim;
	idAnimState				legsAnim;
    // Koz
    idAnimState				leftHandAnim;
    idAnimState				rightHandAnim;
    // Koz end

	bool					allowPain;
	bool					allowEyeFocus;
	bool					finalBoss;

	int						painTime;

	idList<idAttachInfo>	attachments;

	int						solidTest; // HUMANHEAD mdl

	//HUMANHEAD bjk
	int						basePushTime;
	idVec3					basePush;
	int						basePushJoint;
	//HUMANHEAD END

	int						damageCap;

	virtual void			Gib( const idVec3 &dir, const char *damageDefName );
	void					AddBasePush( const idVec3& dir, int location, const idDict* damageDict );	//HUMANHEAD bjk
	void					ApplyBasePush();	//HUMANHEAD bjk

							// removes attachments with "remove" set for when character dies
	void					RemoveAttachments( void );

							// copies animation from body to head joints
	void					CopyJointsFromBodyToHead( void );


private:
protected:			// HUMANHEAD nla - Added so we can access.
	void					SyncAnimChannels( int channel, int syncToChannel, int blendFrames );
	void					FinishSetup( void );
	void					SetupHead( void );


	void					Event_EnableEyeFocus( void );
	void					Event_DisableEyeFocus( void );
	void					Event_Footstep( void );
	void					Event_EnableWalkIK( void );
	void					Event_DisableWalkIK( void );
	void					Event_EnableLegIK( int num );
	void					Event_DisableLegIK( int num );
	void					Event_SetAnimPrefix( const char *name );
	void					Event_LookAtEntity( idEntity *ent, float duration );
	void					Event_PreventPain( float duration );
	void					Event_DisablePain( void );
	void					Event_EnablePain( void );
	void					Event_GetPainAnim( void );
	void					Event_StopAnim( int channel, int frames );
	void					Event_PlayAnim( int channel, const char *name );
	void					Event_PlayCycle( int channel, const char *name );
	void					Event_IdleAnim( int channel, const char *name );
	void					Event_SetSyncedAnimWeight( int channel, int anim, float weight );
	void					Event_OverrideAnim( int channel );
	void					Event_EnableAnim( int channel, int blendFrames );
	void					Event_SetBlendFrames( int channel, int blendFrames );
	void					Event_GetBlendFrames( int channel );
	void					Event_AnimState( int channel, const char *name, int blendFrames );
	void					Event_GetAnimState( int channel );
	void					Event_InAnimState( int channel, const char *name );
	void					Event_FinishAction( const char *name );
	void					Event_AnimDone( int channel, int blendFrames );
	void					Event_HasAnim( int channel, const char *name );
	void					Event_CheckAnim( int channel, const char *animname );
	void					Event_ChooseAnim( int channel, const char *animname );
	void					Event_AnimLength( int channel, const char *animname );
	void					Event_AnimDistance( int channel, const char *animname );
	void					Event_HasEnemies( void );
	void					Event_NextEnemy( idEntity *ent );
	void					Event_ClosestEnemyToPoint( const idVec3 &pos );
	void					Event_StopSound( int channel, int netsync );
	void					Event_SetNextState( const char *name );
	void					Event_SetState( const char *name );
	void					Event_GetState( void );
	void					Event_GetHead( void );
// HUMANHEAD START JRM
	virtual void			Event_Footprint_Left( void );
	virtual void			Event_Footprint_Right( void );
#ifdef HUMANHEAD //jsh
	virtual void			Event_PlayAnimSkip( int channel, const char *name, float skip );
#endif
	void					Event_AFTestSolid(void); // mdl
// HUMANHEAD END

	void					Event_SetDamageGroupScale(const char *groupName, float scale);
	void					Event_SetDamageGroupScaleAll(float scale);
	void					Event_GetDamageGroupScale(const char *groupName);
	void					Event_SetDamageCap(float _damageCap);
	void					Event_SetWaitState(const char *waitState);
	void					Event_GetWaitState();
};

// HUMANHEAD
/*
=====================
idActor::EyeHeightIdeal
=====================
*/
ID_INLINE float idActor::EyeHeightIdeal( void ) const {
	return eyeOffset.z;
}

/*
=====================
idActor::ShouldRemainAlignedToAxial
=====================
*/
ID_INLINE bool idActor::ShouldRemainAlignedToAxial() const {
	return false;//true;
}

/*
=====================
idActor::Give
	HUMANHEAD aob
AOBFIXME: Why is this function inline?  Move to cpp file and get rid of inline. --pdm
=====================
*/
#define DETERMINE_AMOUNT_GIVEN( current, delta, max ) ( (((current) + (delta)) > (max)) ? (max) - (current) : (delta) )
ID_INLINE bool idActor::Give( const char *statname, const char *value) {
	float	deltaAmount = 0.0f;
	int		localMaxHealth = GetMaxHealth();
	int		localHealth = GetHealth();
	bool	processed = false;
	float	localAmountGiven = 0.0f;

	if ( !idStr::Icmp( statname, "health" ) ) {
		deltaAmount = atoi( value );
		localAmountGiven = DETERMINE_AMOUNT_GIVEN( localHealth, deltaAmount, localMaxHealth );
		SetHealth( localHealth + localAmountGiven );
		processed = true;
	}

	return processed;
}
// HUMANHEAD END

#endif /* !__GAME_ACTOR_H__ */
