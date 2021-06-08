#pragma once
enum class EMessageType
{
	ColliderAdded,
	ColliderRemoved,

	MainMenu,
	Credits,
	Options,
	LoadLevel,
	StartGame,
	LevelSelect,
	Quit,
	Resume,

	PlayerHealthChanged,
	PlayerResourceChanged,
	PlayerTakeDamage,
	PlayerDied,
	EnemyDied,
	EnemyHealthChanged,

	IntroStarted,
	IntroEnded,

	UIButtonPress,

	PlayVoiceLine,
	LoadDialogue,
	StopDialogue,

	FadeInComplete,
	FadeOutComplete,

	SetResolution1280x720,
	SetResolution1600x900,
	SetResolution1920x1080,
	SetResolution2560x1440,

	PlayStepSound,
	PlayResearcherReactionExplosives,
	PlayRobotAttackSound,
	PlayRobotDeathSound,
	PlayRobotIdleSound,
	PlayRobotPatrolling,
	PlayRobotSearching,
	PlayVoiceEvent,
	PlaySFX,

	CursorHideAndLock,
	CursorShowAndUnlock,

	ComponentAdded,

	UpdateCrosshair,

	CameraShake,

	ToggleWidget,

	LockPlayer,

	GravityGloveTargetDistance,// Specific, solves single problem. Remove later / Aki 2021 04 29
	GravityGlovePull,
	GravityGlovePush,

	PlayerHealthPickup,
	PlayerSetRespawnPoint,
	PlayerRespawn,

	EnemyPatrolState,
	EnemySeekState,
	EnemyAttackState,
	EnemyAlertedState,
	EnemyIdleState,
	EnemyAttack,
	EnemyTargetLost,
	EnemyTakeDamage,
	EnemyAttackedPlayer,
	EnemyReachedTarget,
	EnemyFoundPlayer,
	EnemyLostPlayer,
	EnemyReachedLastPlayerPosition,

	FuseCreated,
	FusePickedUp,
	OpenSafetyDoors,

	BootUpState,
	PlayJumpSound,
	
	GameStarted,

	LockFPSCamera,

	ReturnToMainMenu,
	EnemyDisabled,

	AddStaticAudioSource,
	ClearStaticAudioSources,
	SetDynamicAudioSource,
	PlayDynamicAudioSource,

	CanvasButtonIndex,
	PauseMenu,
	PhysicsPropCollision,
	PropCollided,

	EnemyStateChange,

	Teleport,

	SetAmbience,

	Play3DVoiceLine,
	Play2DVoiceLine,

	Count
};
