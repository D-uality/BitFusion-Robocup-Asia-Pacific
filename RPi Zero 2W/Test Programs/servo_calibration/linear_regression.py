import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression

# Bot Left
# frequencies = np.array([-400, -300, -200, -150, 150, 200, 300, 400])
# times = np.array([-6, -8.88, -16.67, -30.54, 26.03, 14.89, 8.22, 5.73])

# Top Left
# frequencies = np.array([-400, -300, -200, -150, 150, 200, 300, 400])
# times = np.array([-5.01, -6.9, -11.38, -17.09, 16.91, 11.12, 6.79, 5])

# Top Right
# frequencies = np.array([-400, -300, -200, -150, 150, 200, 300, 400])
# times = np.array([-5.66, -8.06, -14.61, -24.69, 23.27, 13.38, 7.64, 5.32])

# Bot Right
frequencies = np.array([-400, -300, -200, -150, 150, 200, 300, 400])
times = np.array([-5.85, -8.09, -14.51, -24.96, 20.79, 12.85, 7.36, 5.31])

# Calculat the angular speed, omega = 10*pi / T; T is the time taken for 5 rotations
omega = 10 * np.pi / times

# Becase we are trying to find a function that returns a corresponding frequency, inverse
frequencies, omega = omega, frequencies

# Because we want a more human approximation, multiple frequencies by scale factor 100
frequencies = frequencies * 100

# Seperate data into negative and positive groups, as they correspond to backwards and forwards movements
negativeFrequencies = frequencies[times < 0]
negativeOmega = omega[times < 0]
posititiveFrequencies = frequencies[times > 0]
positiveOmega = omega[times > 0]

# Fit a linear regression model for both sets
negativeModel = LinearRegression()
negativeModel.fit(negativeFrequencies.reshape(-1, 1), negativeOmega)
negativeOmegaPredictions = negativeModel.predict(negativeFrequencies.reshape(-1, 1))

positiveModel = LinearRegression()
positiveModel.fit(posititiveFrequencies.reshape(-1, 1), positiveOmega)
positiveOmegaPredictions = positiveModel.predict(posititiveFrequencies.reshape(-1, 1))

# Printing equations
negativeSlope = negativeModel.coef_[0]
negativeIntercept = negativeModel.intercept_
positiveSlope = positiveModel.coef_[0]
positiveIntercept = positiveModel.intercept_

# Create the equation strings
negative_equation = f"y = {negativeSlope:.2f}x + {negativeIntercept:.2f}"
positive_equation = f"y = {positiveSlope:.2f}x + {positiveIntercept:.2f}"

print(f"Negative: {negative_equation} \nPositive: {positive_equation}")

# Adjusting the plots to include the y-intercept and extending the x-axis range for a better view of the intercept
XnegativeExtended = np.linspace(-600, 0, 300)
XpositiveExtended = np.linspace(0, 600, 300)
negativeOmegaExtendedPredicted = negativeModel.predict(XnegativeExtended.reshape(-1, 1))
positiveOmegaExtendedPredicted = positiveModel.predict(XpositiveExtended.reshape(-1, 1))

# Re-plotting with extended axes
fig, ax = plt.subplots(1, 2, figsize=(15, 5))

# Plot for negative frequencies
ax[0].scatter(negativeFrequencies, negativeOmega, color='blue', label='Original data')
ax[0].plot(XnegativeExtended, negativeOmegaExtendedPredicted, color='red', label='Fitted line')
ax[0].set_title('Angular Speed vs. Negative PWM Frequencies')
ax[0].set_xlabel('Angular Frequency (INPUT)')
ax[0].set_ylabel('Corresponding PWM Frequency (OUTPUT)')
ax[0].axhline(0, color='black',linewidth=1.5)
ax[0].axvline(0, color='black',linewidth=1.5)
ax[0].grid(True)
ax[0].legend()

# Plot for positive frequencies
ax[1].scatter(posititiveFrequencies, positiveOmega, color='blue', label='Original data')
ax[1].plot(XpositiveExtended, positiveOmegaExtendedPredicted, color='red', label='Fitted line')
ax[1].set_xlabel('Angular Frequency (INPUT)')
ax[1].set_ylabel('Corresponding PWM Frequency (OUTPUT)')
ax[1].axhline(0, color='black',linewidth=1.5)
ax[1].axvline(0, color='black',linewidth=1.5)
ax[1].grid(True)
ax[1].legend()

plt.tight_layout()
plt.show()