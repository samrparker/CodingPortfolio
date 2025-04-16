//This time, it doesn't have spikes :)

//Written by Samuel Parker

using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;

public class Frisbee : ThrowableObject
{
    //Note: when changing these, I'd highly recommend NOT changing the initial velocity factor, as that can make the frisbee behave in weird ways. I'd recommend changing
    //the drag factor if you want the frisbee to go farther, and the lift factor if you want the frisbee to on average go higher.
    public float initialVelocityFactor; //In testing, this was typically 0.5
    public float dragFactor; //In testing, this was typically 0.3
    public float liftFactor; //In testing, this was typically 5

    private Rigidbody thisBody;
    private Transform thisTransform;

    // Start is called before the first frame update
    protected override void Start()
    {
        base.Start();
        thisBody = GetComponent<Rigidbody>();
        thisTransform = GetComponent<Transform>();
    }
    
    protected override void Update()
    {
        base.Update();

        if (IsInAir())
        {
            //This has no effect on the physics, but generally speaking the factor the rotation of the frisbee, the more lift force, and vice versa. This is purely cosmetic!
            thisBody.AddTorque(transform.up * liftFactor / 2);

            SimulateFrisbeePhysics();

            transform.forward = GetComponent<Rigidbody>().velocity;
        }
    }

    //Applies the frisbee inital velocity factor to the velocity
    protected override void OnThrow()
    {
        GetComponent<Rigidbody>().velocity *= initialVelocityFactor;
    }

    //Gets the angle of attack of the frisbee (compares the y velocity and the horizontal velocity), in degrees
    public float GetAttackAngle()
    {
        //Y as first, interpolated x-z velocity as second Mathf.Atan2 arg!
        float yVelocity = thisBody.velocity.y;
        float xzVelocity = GetXZVelocity();

        return Mathf.Atan2(yVelocity, xzVelocity) * Mathf.Rad2Deg;
    }

    //Gets the angle of attack in radians, is important for calculations!
    public float GetAttackRadians()
    {
        return GetAttackAngle() * Mathf.Deg2Rad;
    }

    //Just gets the interpolated velocity on the x-z plane
    public float GetXZVelocity()
    {
        float xVelocity = thisBody.velocity.x;
        float zVelocity = thisBody.velocity.z;
        return Mathf.Sqrt(xVelocity * xVelocity + zVelocity * zVelocity);
    }

    //Given the angle of the frisbee (from GetAttackAngle function), will calculate a lift force to apply to the frisbee to counter-act the gravity force,
    //and will also decrease the overall velocity by a certain amount due to air resistance.

    //For ALL of the crazy math and numbers, I'd like to thank V. R. Morrison for publishing their paper that you can find here: https://docs.google.com/viewer?url=http:%2F%2Fweb.mit.edu%2Fwomens-ult%2Fwww%2Fsmite%2Ffrisbee_physics.pdf
    //I got from that paper the equations you see below, with a slightly different format.
    public void SimulateFrisbeePhysics()
    {
        float attackAngle = GetAttackAngle();
        float xzVelocity = GetXZVelocity();

        //Don't question these numbers!!!!!!!!!!!!!
        float totalLift = 1.4f * GetAttackRadians();

        float liftForce = 1.23f * Mathf.Pow(xzVelocity, 2) * 0.0568f * totalLift / 12;

        float drag = 0.08f + 2.72f * Mathf.Pow(GetAttackRadians(), 2);

        float dragForce = 1.23f * Mathf.Pow(xzVelocity, 1.9f) * 0.0568f * drag;

        thisBody.AddForce(Vector3.up * liftForce * thisBody.mass * liftFactor, ForceMode.Acceleration);
        thisBody.AddForce(-1.0f * transform.forward * dragForce * thisBody.mass * dragFactor, ForceMode.Acceleration);
    }
}