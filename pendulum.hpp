#pragma once

#include <cmath>
#include <cmdfx.hpp>

class Pendulum {
	private:
		double length;
		double mass;
		double angle;
		// x and y are the pivot point coordinates
		int x;
		int y;
		double angularVelocity;
		double angularAcceleration;

		Pendulum* parent;

		int color;
		bool draw; // whether to leave a trail at the ball position

		// Previous ball position for erasing
		int prevBallX;
		int prevBallY;
        int prevPivotX;
        int prevPivotY;

		// global parameters (shared)
		static inline double globalDamping = 0.01;
		static inline double globalCoupling = 0.5;

		// utility methods for consistent drawing/erasing
		static void erasePendulum(int pivotX, int pivotY, int ballX, int ballY, bool drawTrail) {
			CmdFX::Canvas::line(pivotX, pivotY, ballX, ballY, ' ');
			if (!drawTrail) {
				CmdFX::Canvas::fillCircle(ballX, ballY, 1, ' ');
			}
		}

		static void drawPendulum(int pivotX, int pivotY, int ballX, int ballY, int color) {
			CmdFX::Canvas::setForeground(color);
			CmdFX::Canvas::line(pivotX, pivotY, ballX, ballY, '+');
			CmdFX::Canvas::fillCircle(ballX, ballY, 1, '@');
			CmdFX::Canvas::resetFormat();
		}

		void update(double gravity, double timeStep) {
			// basic gravity term
			double accel = (-gravity / length) * sin(angle);

			// simple coupling to parent (not full multi-body physics)
			if (parent != nullptr) {
				const double coupling = 0.5; // torque coupling factor
				accel += coupling * (parent->angle - angle);
			}

			angularAcceleration = accel;
			// simple damping
			const double damping = 0.01;
			angularVelocity += angularAcceleration * timeStep;
			angularVelocity *= (1.0 - damping * timeStep);
			angle += angularVelocity * timeStep;
		}

	public:
		Pendulum(double length, double mass, double angle, int x, int y, int color = 0xFFFFFF, bool draw = false)
			: length(length), mass(mass), angle(angle),
			  x(x), y(y),angularVelocity(0.0), angularAcceleration(0.0), parent(nullptr), color(color), draw(draw) {
			prevBallX = x + static_cast<int>(length * sin(angle));
			prevBallY = y + static_cast<int>(length * cos(angle));
			prevPivotX = x;
			prevPivotY = y;
		}

		Pendulum* getParent() const {
			return parent;
		}

		void setParent(Pendulum* p) {
			parent = p;
			x = p->getBallX();
			y = p->getBallY();
		}

		double getLength() const {
			return length;
		}

		double getMass() const {
			return mass;
		}

		int getX() const {
			return x;
		}

		int getY() const {
			return y;
		}

		int getBallX() const {
			return x + static_cast<int>(length * sin(angle));
		}

		int getBallY() const {
			return y + static_cast<int>(length * cos(angle));
		}

		void setPos(int x, int y) {
			this->x = x;
			this->y = y;
		}

		double getAngle() const {
			return angle;
		}

		void setAngle(double angle) {
			this->angle = angle;
		}

		double getAngularVelocity() const {
			return angularVelocity;
		}

		void setAngularVelocity(double angularVelocity) {
			this->angularVelocity = angularVelocity;
		}

		double getAngularAcceleration() const {
			return angularAcceleration;
		}

		void setAngularAcceleration(double angularAcceleration) {
			this->angularAcceleration = angularAcceleration;
		}

		void step(double gravity, double timeStep) {
			// compute current pivot (attach to parent if present)
			int pivotX = x;
			int pivotY = y;
			if (parent != nullptr) {
				pivotX = parent->getBallX();
				pivotY = parent->getBallY();
				x = pivotX;
				y = pivotY;
			}

			// erase previous pendulum (use previous pivot and ball)
			erasePendulum(prevPivotX, prevPivotY, prevBallX, prevBallY, draw);

			update(gravity, timeStep);

			// compute new ball position relative to current pivot
			int ballX = pivotX + static_cast<int>(length * sin(angle));
			int ballY = pivotY + static_cast<int>(length * cos(angle));

			// draw pendulum
			drawPendulum(pivotX, pivotY, ballX, ballY, color);

			prevPivotX = pivotX;
			prevPivotY = pivotY;
			prevBallX = ballX;
			prevBallY = ballY;
	}

	static void setGlobalDamping(double d) { globalDamping = d; }
	static void setGlobalCoupling(double c) { globalCoupling = c; }

	static void stepMultiPendulum(std::vector<Pendulum*> pendulums, double gravity, double timeStep) {
		if (pendulums.empty()) return;

		size_t n = pendulums.size();

		// Determine root pivot
		int rootPivotX = pendulums[0]->x;
		int rootPivotY = pendulums[0]->y;
		if (pendulums[0]->parent != nullptr) {
			rootPivotX = pendulums[0]->parent->getBallX();
			rootPivotY = pendulums[0]->parent->getBallY();
			pendulums[0]->x = rootPivotX;
			pendulums[0]->y = rootPivotY;
		}

		// Erase all previous drawings
		for (size_t i = 0; i < n; i++) {
			erasePendulum(pendulums[i]->prevPivotX, pendulums[i]->prevPivotY,
			              pendulums[i]->prevBallX, pendulums[i]->prevBallY, pendulums[i]->draw);
		}

		// for chains of 3+, use iterative approximation of coupled equations
		// for exactly 2, use precise double pendulum equations
		if (n == 2) {
			double a = pendulums[0]->angle;
			double b = pendulums[1]->angle;
			double adot = pendulums[0]->angularVelocity;
			double bdot = pendulums[1]->angularVelocity;
			double m1 = pendulums[0]->mass;
			double m2 = pendulums[1]->mass;
			double l1 = pendulums[0]->length;
			double l2 = pendulums[1]->length;

			double delta = a - b;
			double denom = 2*m1 + m2 - m2 * cos(2*delta);
			if (denom == 0.0) denom = 1e-6;

			double gravityTerm1 = -gravity * (2*m1 + m2) * sin(a);
			double gravityTerm2 = -m2 * gravity * sin(a - 2*b);
			double couplingTerm = -2 * sin(delta) * m2 * (bdot*bdot*l2 + adot*adot*l1*cos(delta));
			double numerator1 = gravityTerm1 + gravityTerm2 + couplingTerm;
			double add = numerator1 / (l1 * denom);

			double angularMomentumTerm = adot*adot * l1 * (m1 + m2);
			double gravityTermB = gravity * (m1 + m2) * cos(a);
			double centrifugalTerm = bdot*bdot * l2 * m2 * cos(delta);
			double numerator2 = 2 * sin(delta) * (angularMomentumTerm + gravityTermB + centrifugalTerm);
			double bdd = numerator2 / (l2 * denom);

			pendulums[0]->angularAcceleration = add;
			pendulums[1]->angularAcceleration = bdd;
		} else {
			// for n >= 3, use Lagrangian-based multi-body dynamics with constraint forces
			std::vector<double> accelerations(n);

			// compute constraint forces and accelerations using iterative method
			for (size_t i = 0; i < n; i++) {
				double li = pendulums[i]->length;
				double mi = pendulums[i]->mass;
				double thetai = pendulums[i]->angle;
				double omegai = pendulums[i]->angularVelocity;

				// base gravitational acceleration
				double accel = (-gravity / li) * sin(thetai);

				// calculate total suspended mass below this pendulum
				double totalMassBelow = 0.0;
				for (size_t j = i + 1; j < n; j++) {
					totalMassBelow += pendulums[j]->mass;
				}

				// constraint acceleration from masses below (Lagrangian constraint force)
				if (totalMassBelow > 0.0) {
					double constraintAccel = 0.0;

					// sum constraint forces from all pendulums below
					for (size_t j = i + 1; j < n; j++) {
						double lj = pendulums[j]->length;
						double mj = pendulums[j]->mass;
						double thetaj = pendulums[j]->angle;
						double omegaj = pendulums[j]->angularVelocity;

						// calculate position differences for constraint force
						double deltaTheta = thetaj - thetai;
						double pathLength = 1.0; // cumulative path from i to j
						for (size_t k = i; k < j; k++) {
							pathLength += pendulums[k]->length / li;
						}

						// constraint force proportional to mass and distance
						double massWeight = mj / (mi + totalMassBelow);
						double constraintTerm = massWeight * (lj / li) / (pathLength * pathLength);

						// centripetal acceleration constraint
						constraintAccel += constraintTerm * omegaj * omegaj * sin(deltaTheta);

						// angular coupling constraint with damping
						constraintAccel += globalCoupling * constraintTerm * sin(deltaTheta) * 0.5;
					}

					accel += constraintAccel;
				}

				// coupling with adjacent pendulums (nearest neighbor interaction)
				if (i > 0) {
					double thetaPrev = pendulums[i-1]->angle;
					double omegaPrev = pendulums[i-1]->angularVelocity;
					double lPrev = pendulums[i-1]->length;
					double mPrev = pendulums[i-1]->mass;

					// parent-child constraint acceleration
					double deltaFromParent = thetaPrev - thetai;
					double parentCouplingStrength = (mPrev / (mi + mPrev)) * (lPrev / li);
					accel += parentCouplingStrength * omegaPrev * omegaPrev * sin(deltaFromParent) * 0.3;
				}

				if (i + 1 < n) {
					double thetaNext = pendulums[i+1]->angle;
					double omegaNext = pendulums[i+1]->angularVelocity;
					double lNext = pendulums[i+1]->length;
					double mNext = pendulums[i+1]->mass;

					// child-parent reaction acceleration
					double deltaToChild = thetaNext - thetai;
					double childCouplingStrength = (mNext / (mi + mNext)) * (lNext / li);
					accel += childCouplingStrength * omegaNext * omegaNext * sin(deltaToChild) * 0.15;
				}

				accelerations[i] = accel;
			}

			// apply calculated accelerations
			for (size_t i = 0; i < n; i++) {
				pendulums[i]->angularAcceleration = accelerations[i];
			}
		}

		// integrate velocities and angles for all pendulums
		for (size_t i = 0; i < n; i++) {
			pendulums[i]->angularVelocity += pendulums[i]->angularAcceleration * timeStep;
			pendulums[i]->angularVelocity *= (1.0 - globalDamping * timeStep);
			pendulums[i]->angle += pendulums[i]->angularVelocity * timeStep;
		}

		// compute positions and draw all pendulums
		int currentPivotX = rootPivotX;
		int currentPivotY = rootPivotY;

		for (size_t i = 0; i < n; i++) {
			int ballX = currentPivotX + static_cast<int>(pendulums[i]->length * sin(pendulums[i]->angle));
			int ballY = currentPivotY + static_cast<int>(pendulums[i]->length * cos(pendulums[i]->angle));

			drawPendulum(currentPivotX, currentPivotY, ballX, ballY, pendulums[i]->color);

			pendulums[i]->prevPivotX = currentPivotX;
			pendulums[i]->prevPivotY = currentPivotY;
			pendulums[i]->prevBallX = ballX;
			pendulums[i]->prevBallY = ballY;

			// update internal coordinates
			pendulums[i]->x = currentPivotX;
			pendulums[i]->y = currentPivotY;

			// next pendulum pivots from this ball
			currentPivotX = ballX;
			currentPivotY = ballY;
		}
	}
};
