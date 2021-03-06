//-----------------------------------------------------------------------------
//
// TurokAIAnimal.js
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

TurokAIAnimal = class.extendStatic(ComponentTurokAI);

class.properties(TurokAIAnimal,
{
    //------------------------------------------------------------------------
    // VARS
    //------------------------------------------------------------------------
    
    extendedRadius  : 2.5,
    
    //------------------------------------------------------------------------
    // FUNCTIONS
    //------------------------------------------------------------------------
    
    walk : function(actor)
    {
        this.state = AI_STATE_WALKING;
        actor.blendAnim(AI_ANIM_RUNNING, 4.0, 8.0,
            NRender.ANIM_LOOP |
            NRender.ANIM_ROOTMOTION);
    },
    
    run : function(actor)
    {
        this.state = AI_STATE_RUNNING;
        actor.blendAnim(AI_ANIM_RUNNING, 4.0, 8.0,
            NRender.ANIM_LOOP |
            NRender.ANIM_ROOTMOTION);
    },
    
    turn : function(actor, angles)
    {
        if(actor.ai.bTurning)
            return;
        
        actor.ai.setIdealYaw(actor.yaw + angles, 4.096);
    },
    
    //------------------------------------------------------------------------
    // EVENTS
    //------------------------------------------------------------------------
    
    onTargetFound : function()
    {
        var actor = this.parent.owner;
        var ai = actor.ai;
        
        if(ai.getTargetDistance() >= 768.0)
        {
            ai.clearTarget();
            return;
        }
        
        this.run(actor);
        this.turn(actor, ai.yawToTarget() + Math.PI);
    },
    
    onSleep : function()
    {
        var actor = this.parent.owner;
        
        this.idle(actor);
        actor.ai.clearTarget();
    },
    
    onWake : function()
    {
        this.idle(this.parent.owner);
    },
    
    onThink : function()
    {
        var ai = this.parent.owner.ai;
        var actor = ai.owner;
        
        if(ai.bSeeTarget)
        {
            this.sightThreshold++;
            if(this.sightThreshold > 50)
                this.sightThreshold = 50;
        }
        
        // bump against walls
        if(ai.bAvoidWalls && !actor.ai.bTurning)
        {
            if(!ai.checkPosition(this.extendedRadius, actor.yaw + 0.5236)) {
                this.turn(actor, -0.909);
            }
            else if(!ai.checkPosition(this.extendedRadius, actor.yaw - 0.5236)) {
                this.turn(actor, 0.909);
            }
        }
        
        if(ai.target != null)
        {
            switch(this.state)
            {
            case AI_STATE_STANDING:
                break;
            case AI_STATE_RUNNING:
                if(!ai.bSeeTarget && (this.sightThreshold-- <= 0) ||
                    ai.getTargetDistance() >= 768.0)
                {
                    ai.clearTarget();
                    this.idle(actor);
                }
                break;
            case AI_STATE_DEATH:
                break;
            }
        }
    },
    
    onDamage : function(instigator)
    {
        this.dropPickup('pickup_health_small');
    },
    
    onReady : function()
    {
        var actor = this.parent.owner;
        
        ComponentTurokAI.prototype.onReady.bind(this)();
        actor.ai.bLookAtTarget = false;
        actor.ai.nodeHead = 0;
    },
    
    onDeath : function(instigator)
    {
        if(this.state == AI_STATE_DEATH)
            return;
        
        ComponentTurokAI.prototype.onDeath.bind(this)();
        
        var actor = this.parent.owner;
        
        this.dropPickup('pickup_health_small');
        this.dropPickup('pickup_health_small');
        this.dropPickup('pickup_health_small');
        
        Snd.play('sounds/shaders/generic_3_energy_pickup.ksnd', actor);
        Sys.spawnFx('fx/animal_death_flash.kfx', null,
            actor.getLocalVector(0, 51.2, actor.height), actor.rotation,
            Plane.fromIndex(actor.plane));
        
        GameActor.remove(actor);
    },
});
